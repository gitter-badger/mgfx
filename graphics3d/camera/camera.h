#pragma once

struct Ray
{
    glm::vec3 position;
    glm::vec3 direction;
};

// A simple camera
class Camera
{
public:
    Camera();
    virtual ~Camera();


    void SetPositionAndFocalPoint(const glm::vec3& pos, const glm::vec3& point);

    // Pixel size of the rendering
    void SetFilmSize(const glm::uvec2& size);
    glm::uvec2 GetFilmSize() const { return filmSize; }

    // Angle of view
    void SetFieldOfView(float fov) { fieldOfView = fov; }
    float GetFieldOfView() const { return fieldOfView; }

    // Called to update the camera state for a given window area
    bool Update(SDL_Window* pWindow);

    // Calculated matrices
    glm::mat4 GetLookAt() const;
    glm::mat4 GetProjection() const;

    // Pos/View Dir
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetViewDirection() const { return viewDirection; }

    // A ray into the world through a screen pixel
    Ray GetWorldRay(const glm::vec2& imageSample);

    // Standard manipulation functions
    void Walk(glm::vec3 planes);
    void Dolly(float distance);
    void Orbit(const glm::vec2& angle);

private:

    // Refresh state
    void UpdateRightUp();
    void UpdateWalk(int64_t timeDelta);
    void UpdatePosition(int64_t timeDelta);
    void UpdateOrbit(int64_t timeDelta);

private:
    glm::vec3 position = glm::vec3(0.0f);                          // Position of the camera in world space
    glm::vec3 focalPoint = glm::vec3(0.0f);                        // Look at point

    // Size of the camera film
    glm::uvec2 filmSize = glm::uvec2(0, 0);

    glm::vec3 viewDirection = glm::vec3(0.0f, 0.0f, 1.0f);         // The direction the camera is looking in
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);                 // The vector to the right
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);                    // The vector up

    float fieldOfView = 60.0f;                                      // Field of view
    float halfAngle = 30.0f;                                        // Half angle of the view frustum
    float aspectRatio = 1.0f;                                       // Ratio of x to y of the viewport

    glm::quat orientation = glm::quat();                            // A quaternion representing the camera rotation

    glm::vec2 orbitDelta = glm::vec2(0.0f);
    glm::vec3 positionDelta = glm::vec3(0.0f);
    glm::vec3 walkDelta = glm::vec3(0.0f);

    int64_t lastTime = 0;
};
