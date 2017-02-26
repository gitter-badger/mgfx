#pragma once

class Camera;
class Mesh;
struct IDevice;

class Scene
{
public:
    Scene();
    const glm::vec4& GetClearColor() const { return m_clearColor; }
    void SetClearColor(const glm::vec4& col) { m_clearColor = col; }

    void AddMesh(std::shared_ptr<Mesh>& spMesh) { m_vecMeshes.push_back(spMesh); }
    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_vecMeshes; }

    void AddCamera(std::shared_ptr<Camera>& spCamera) { m_vecCameras.push_back(spCamera); }
    const std::vector<std::shared_ptr<Camera>>& GetCameras(std::shared_ptr<Camera>& spCamera) const { return m_vecCameras; }
    void Render(IDevice* pDevice);

private:
    glm::vec4 m_clearColor;
    std::shared_ptr<Camera> m_spCurrentCamera;
    std::vector<std::shared_ptr<Camera>> m_vecCameras;
    std::vector<std::shared_ptr<Mesh>> m_vecMeshes;
};