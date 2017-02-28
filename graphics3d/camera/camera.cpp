#include "Common.h"
#include "Camera.h"
#include "Timer.h"

void Camera::SetPositionAndFocalPoint(const glm::vec3& pos, const glm::vec3& point)
{
    // From
    m_position = pos;

    // Focal
    m_focalPoint = point;

    // Work out direction
    m_viewDirection = m_focalPoint - m_position;
    m_viewDirection = glm::normalize(m_viewDirection);

    // Get camera orientation relative to -z
    m_orientation = QuatFromVectors(m_viewDirection, glm::vec3(0.0f, 0.0f, -1.0f));
    m_orientation = glm::normalize(m_orientation);

    UpdateRightUp();
}

void Camera::SetFilmSize(const glm::uvec2& size)
{
    m_filmSize = size;
    m_aspectRatio = size.x / float(size.y);
}


bool Camera::Update()
{
    // The half-width of the viewport, in world space
    m_halfAngle = float(tan(glm::radians(m_fieldOfView) / 2.0));

    auto time = Timer::Instance().GetTimeMs();

    int64_t delta;
    if (m_lastTime == 0)
    {
        m_lastTime = time;
        delta = 0;
    }
    else
    {
        delta = time - m_lastTime;
        m_lastTime = time;
    }
    //delta = 1000;

    bool changed = false;
    if (m_orbitDelta != glm::vec2(0.0f))
    {
        UpdateOrbit(delta);
        changed = true;
    }

    if (m_positionDelta != glm::vec3(0.0f))
    {
        UpdatePosition(delta);
        changed = true;
    }

    if (m_walkDelta != glm::vec3(0.0f))
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
    auto lensRand = glm::circularRand(0.0f);

    auto dir = m_viewDirection;
    float x = ((imageSample.x * 2.0f) / m_filmSize.x) - 1.0f;
    float y = ((imageSample.y * 2.0f) / m_filmSize.y) - 1.0f;

    // Take the view direction and adjust it to point at the given sample, based on the 
    // the frustum 
    dir += (m_right * (m_halfAngle * m_aspectRatio * x));
    dir -= (m_up * (m_halfAngle * y));
    //dir = normalize(dir);
    float ft = (glm::length(m_focalPoint - m_position) - 1.0f) / glm::length(dir);
    glm::vec3 focasPoint = m_position + dir * ft;

    glm::vec3 lensPoint = m_position;
    lensPoint += (m_right * lensRand.x);
    lensPoint += (m_up * lensRand.y);
    dir = glm::normalize(focasPoint - lensPoint);

    return Ray{ lensPoint, dir };
}

void Camera::Walk(glm::vec3 planes)
{
    planes *= 2.0f;
    m_walkDelta += m_viewDirection * planes.z;
    m_walkDelta += m_right * planes.x;
    m_walkDelta += m_up * planes.y;
}

void Camera::Dolly(float distance)
{
    m_positionDelta += m_viewDirection * distance;
}

// Orbit around the focal point, keeping y 'Up'
void Camera::Orbit(const glm::vec2& angle)
{
    m_orbitDelta += angle;
}

void Camera::UpdateWalk(int64_t timeDelta)
{
    const float settlingTimeMs = 50;
    float frac = std::min(timeDelta / settlingTimeMs, 1.0f);
    frac = SmoothStep(frac);
    glm::vec3 distance = frac * m_walkDelta;
    m_walkDelta *= (1.0f - frac);

    m_position += distance;
    m_focalPoint += distance;
}

void Camera::UpdatePosition(int64_t timeDelta)
{
    const float settlingTimeMs = 50;
    float frac = std::min(timeDelta / settlingTimeMs, 1.0f);
    frac = SmoothStep(frac);
    glm::vec3 distance = frac * m_positionDelta;
    m_positionDelta *= (1.0f - frac);

    m_position += distance;
}

void Camera::UpdateOrbit(int64_t timeDelta)
{
    const float settlingTimeMs = 50;
    float frac = std::min(timeDelta / settlingTimeMs, 1.0f);
    frac = SmoothStep(frac);

    // Get a proportion of the remaining turn angle, based on the time delta
    glm::vec2 angle = frac * m_orbitDelta;

    // Reduce the orbit delta remaining for next time
    m_orbitDelta *= (1.0f - frac);
    if (glm::all(glm::lessThan(glm::abs(m_orbitDelta), glm::vec2(.1f))))
    {
        m_orbitDelta = glm::vec2(0.0f);
    }

    // 2 rotations, about right and world up, for the camera
    glm::quat rotY = glm::angleAxis(glm::radians(angle.y), glm::vec3(m_right));
    glm::quat rotX = glm::angleAxis(glm::radians(angle.x), glm::vec3(0.0f, 1.0f, 0.0f));

    // Concatentation of the current rotations with the new one
    m_orientation = m_orientation * rotY * rotX;
    m_orientation = glm::normalize(m_orientation);

    // Recalculate position from the new view direction, relative to the focal point
    float distance = glm::length(m_focalPoint - m_position);
    m_viewDirection = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f) * m_orientation);
    m_position = m_focalPoint - (m_viewDirection * distance);

    UpdateRightUp();
}

// Matrices
glm::mat4 Camera::GetLookAt() const
{
    glm::vec3 up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) * m_orientation);
    return glm::lookAt(m_position, m_focalPoint, up);
}

glm::mat4 Camera::GetProjection() const
{
    // TODO; ZClip
    return glm::perspectiveFov(glm::radians(m_fieldOfView), float(m_filmSize.x), float(m_filmSize.y), .1f, 10000.0f);
}

void Camera::UpdateRightUp()
{
    // Right and up vectors updated based on the quaternion orientation
    m_right = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f) * m_orientation);
    m_up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) * m_orientation);
}
