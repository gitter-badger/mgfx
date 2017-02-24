#pragma once


#include "gl3w.h"
#include "imgui_impl_sdl_gl3.h"
#include "gl3w.h"
#include "shader.h"
#include "texture.h"
#include "IDevice.h"

class Camera;
class Mesh;
struct MeshPart;
class Scene;
struct SDL_Window;
union SDL_Event;

struct GLMeshPart
{
    uint32_t positionID = 0;
    uint32_t normalID = 0;
    uint32_t uvID = 0;
    uint32_t numVertices = 0;
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
    virtual bool Init(std::shared_ptr<Scene>& spScene) override;
    virtual bool PreRender() override;
    virtual bool Render() override;
    virtual void Prepare2D() override;
    virtual void Cleanup() override;
    virtual void ProcessEvent(SDL_Event& event) override;
    virtual void Swap() override;
    virtual SDL_Window* GetWindow() const override { return pWindow; }

    virtual void Draw(Mesh* pMesh) override;

private:
    std::shared_ptr<GLMesh> BuildDeviceMesh(Mesh* pMesh);
    uint32_t LoadTexture(std::string path);
    void DestroyDeviceMesh(GLMesh* pDeviceMesh);
    void DestroyDeviceMeshes();

private:
    std::shared_ptr<Scene> m_spScene;

    std::map<Mesh*, std::shared_ptr<GLMesh>> m_mapDeviceMeshes;
    std::map<std::string, uint32_t> m_mapTexToID;

    SDL_Window* pWindow = nullptr;
    SDL_GLContext glContext;

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
