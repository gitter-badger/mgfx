#pragma once

class Camera;

class Scene
{
public:
    Scene();
    const glm::vec4& GetClearColor() const { return m_clearColor; }
    void SetClearColor(const glm::vec4& col) { m_clearColor = col; }

    std::shared_ptr<Camera>& GetCurrentCamera() { return m_spCurrentCamera; }
    void SetCurrentCamera(std::shared_ptr<Camera>& spCamera) { m_spCurrentCamera = spCamera; }

private:
    glm::vec4 m_clearColor;
    std::shared_ptr<Camera> m_spCurrentCamera;
    std::vector<std::shared_ptr<Camera>> m_vecCameras;
};