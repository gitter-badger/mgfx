#pragma once


#include "gl3w.h"
#include "imguisdl_gl3.h"
#include "gl3w.h"
#include "shader.h"
#include "IDevice.h"

class Camera;
class Mesh;
class Scene;
class Window;
struct WindowData;
struct MeshPart;

struct SDL_Window;
union SDL_Event;

struct GLMeshPart
{
    uint32_t positionID = 0;
    uint32_t normalID = 0;
    uint32_t tangentID = 0;
    uint32_t binormalID = 0;
    uint32_t uvID = 0;
    uint32_t indicesID = 0;
    uint32_t numIndices = 0;
    uint32_t textureID = 0;
    uint32_t textureIDBump = 0;
};

struct GLMesh
{
    std::map<MeshPart*, std::shared_ptr<GLMeshPart>> m_glMeshParts;
};

class DeviceGL : public IDevice
{
public:
    DeviceGL();
    ~DeviceGL();
    virtual bool Init(std::shared_ptr<Scene>& spScene) override;
    virtual bool Render() override;
    virtual void Prepare2D() override;
    virtual void Render2D() override;
    virtual void Cleanup() override;
    virtual void ProcessEvent(SDL_Event& event) override;
    virtual void Swap() override;
    virtual SDL_Window* GetWindow() const override { return pSDLWindow; }

    virtual void Draw(Mesh* pMesh) override;

private:
    std::shared_ptr<GLMesh> BuildDeviceMesh(Mesh* pMesh);
    uint32_t LoadTexture(const fs::path& path);
    void DestroyDeviceMesh(GLMesh* pDeviceMesh);
    void DestroyDeviceMeshes();

private:
    std::shared_ptr<Scene> m_spScene;

    std::map<Mesh*, std::shared_ptr<GLMesh>> m_mapDeviceMeshes;
    std::map<fs::path, uint32_t> m_mapTexToID;

    SDL_Window* pSDLWindow = nullptr;
    SDL_GLContext glContext;

    std::shared_ptr<ImGuiSDL_GL3> m_spImGuiDraw;

    uint32_t VertexArrayID = 0;

    uint32_t programID = 0;

    uint32_t MatrixID = 0;
    uint32_t ViewMatrixID = 0;
    uint32_t ModelMatrixID = 0;

    uint32_t TextureID = 0; 
    uint32_t HasNormalMapID = 0;
    uint32_t TextureIDBump = 0; 

    uint32_t CameraID = 0;
    uint32_t LightDirID = 0;

};

inline void CheckGL(const char* call, const char* file, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        LOG(ERROR) << std::hex << err << file << ":" << line << call;
    }
}
#define CHECK_GL(stmt) do { stmt; CheckGL(#stmt, __FILE__, __LINE__);  } while (0)
