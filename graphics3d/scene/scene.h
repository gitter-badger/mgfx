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

    std::shared_ptr<Camera>& GetCurrentCamera() { return m_spCurrentCamera; }
    void SetCurrentCamera(std::shared_ptr<Camera>& spCamera) { m_spCurrentCamera = spCamera; }

    void AddMesh(std::shared_ptr<Mesh>& spMesh) { m_vecMeshes.push_back(spMesh); }
    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_vecMeshes; }

    void Render(IDevice* pDevice);

private:
    glm::vec4 m_clearColor;
    std::shared_ptr<Camera> m_spCurrentCamera;
    std::vector<std::shared_ptr<Camera>> m_vecCameras;
    std::vector<std::shared_ptr<Mesh>> m_vecMeshes;
};