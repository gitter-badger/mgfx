#include "Common.h"
#include "Camera.h"

namespace
{

int64_t GetTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

float SmoothStep(float val)
{
    return val * val * (3.0f - 2.0f * val);
}

}

Camera::Camera()
{
}

Camera::~Camera()
{

}

void Camera::SetPositionAndFocalPoint(const glm::vec3& pos, const glm::vec3& point)
{
    // From
    position = pos;

    // Focal
    focalPoint = point;

    // Work out direction
    viewDirection = focalPoint - position;
    viewDirection = glm::normalize(viewDirection);

    // Get camera orientation relative to -z
    orientation = QuatFromVectors(viewDirection, glm::vec3(0.0f, 0.0f, -1.0f));
    orientation = glm::normalize(orientation);

    UpdateRightUp();
}

void Camera::SetFilmSize(const glm::uvec2& size)
{
    filmSize = size;
    aspectRatio = size.x / float(size.y);
}


bool Camera::PreRender()
{
    // The half-width of the viewport, in world space
    halfAngle = float(tan(glm::radians(fieldOfView) / 2.0));

    auto time = GetTime();

    int64_t delta;
    if (lastTime == 0)
    {
        lastTime = time;
        delta = 0;
    }
    else
    {
        delta = time - lastTime;
        lastTime = time;
    }

    bool changed = false;
    if (orbitDelta != glm::vec2(0.0f))
    {
        UpdateOrbit(delta);
        changed = true;
    }

    if (positionDelta != glm::vec3(0.0f))
    {
        UpdatePosition(delta);
        changed = true;
    }

    if (walkDelta != glm::vec3(0.0f))
    {
        UpdateWalk(delta);
        changed = true;
    }

    UpdateRightUp();
    return changed;
}


// Given a screen coordinate, return a ray leaving the camera and entering the world at that 'pixel'
Ray Camera::GetWorldRay(const glm::vec2& imageSample)
{
    // Could move some of this maths out of here for speed, but this isn't time critical
    auto lensRand = glm::circularRand(0.34f);

    auto dir = viewDirection;
    float x = ((imageSample.x * 2.0f) / filmSize.x) - 1.0f;
    float y = ((imageSample.y * 2.0f) / filmSize.y) - 1.0f;

    // Take the view direction and adjust it to point at the given sample, based on the 
    // the frustum 
    dir += (right * (halfAngle * aspectRatio * x));
    dir -= (up * (halfAngle * y));
    //dir = normalize(dir);
    float ft = (glm::length(focalPoint - position) - 1.0f) / glm::length(dir);
    glm::vec3 focasPoint = position + dir * ft;

    glm::vec3 lensPoint = position;
    lensPoint += (right * lensRand.x);
    lensPoint += (up * lensRand.y);
    dir = glm::normalize(focasPoint - lensPoint);

    return Ray{ lensPoint, dir };
}

void Camera::Walk(glm::vec3 planes)
{
    walkDelta += viewDirection * planes.z;
    walkDelta += right * planes.x;
    walkDelta += up * planes.y;
}

void Camera::Dolly(float distance)
{
    positionDelta += viewDirection * distance;
}

// Orbit around the focal point, keeping y 'Up'
void Camera::Orbit(const glm::vec2& angle)
{
    orbitDelta += angle;
}

void Camera::UpdateWalk(int64_t timeDelta)
{
    const float settlingTimeMs = 50;
    float frac = std::min(timeDelta / settlingTimeMs, 1.0f);
    frac = SmoothStep(frac);
    glm::vec3 distance = frac * walkDelta;
    walkDelta *= (1.0f - frac);

    position += walkDelta;
    focalPoint += walkDelta;
}

void Camera::UpdatePosition(int64_t timeDelta)
{
    const float settlingTimeMs = 50;
    float frac = std::min(timeDelta / settlingTimeMs, 1.0f);
    frac = SmoothStep(frac);
    glm::vec3 distance = frac * positionDelta;
    positionDelta *= (1.0f - frac);

    position += distance;
}

void Camera::UpdateOrbit(int64_t timeDelta)
{
    const float settlingTimeMs = 50;
    float frac = std::min(timeDelta / settlingTimeMs, 1.0f);
    frac = SmoothStep(frac);

    // Get a proportion of the remaining turn angle, based on the time delta
    glm::vec2 angle = frac * orbitDelta;

    // Reduce the orbit delta remaining for next time
    orbitDelta *= (1.0f - frac);
    if (glm::all(glm::lessThan(glm::abs(orbitDelta), glm::vec2(.1f))))
    {
        orbitDelta = glm::vec2(0.0f);
    }

    // 2 rotations, about right and world up, for the camera
    glm::quat rotY = glm::angleAxis(glm::radians(angle.y), glm::vec3(right));
    glm::quat rotX = glm::angleAxis(glm::radians(angle.x), glm::vec3(0.0f, 1.0f, 0.0f));

    // Concatentation of the current rotations with the new one
    orientation = orientation * rotY * rotX;
    orientation = glm::normalize(orientation);

    // Recalculate position from the new view direction, relative to the focal point
    float distance = glm::length(focalPoint - position);
    viewDirection = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f) * orientation);
    position = focalPoint - (viewDirection * distance);

    UpdateRightUp();

}

// Matrices
glm::mat4 Camera::GetLookAt() const
{
    glm::vec3 up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) * orientation);
    return glm::lookAt(position, focalPoint, up);
}

glm::mat4 Camera::GetProjection() const
{
    // TODO; ZClip
    return glm::perspectiveFov(glm::radians(fieldOfView), float(filmSize.x), float(filmSize.y), .1f, 10000.0f);
}

void Camera::UpdateRightUp()
{
    // Right and up vectors updated based on the quaternion orientation
    right = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f) * orientation);
    up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) * orientation);
}
