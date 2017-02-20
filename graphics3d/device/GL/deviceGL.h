#pragma once


#include "gl3w.h"
#include "imgui_impl_sdl_gl3.h"
#include "gl3w.h"
#include "shader.h"
#include "texture.h"
#include "IDevice.h"

class Camera;
class Scene;
struct SDL_Window;
union SDL_Event;

struct GLMesh
{
    uint32_t VertexArrayID = 0;
    uint32_t vertexbuffer = 0;
    uint32_t uvbuffer = 0;
    uint32_t normalbuffer = 0;
    uint32_t numVertices = 0;
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
    void DestroyDeviceMesh(GLMesh* pDeviceMesh);
    void DestroyDeviceMeshes();

private:
    std::shared_ptr<Scene> m_spScene;

    std::map<Mesh*, std::shared_ptr<GLMesh>> m_mapDeviceMeshes;

    SDL_Window* pWindow = nullptr;
    SDL_GLContext glContext;


    uint32_t programID = 0;

    uint32_t MatrixID = 0;
    uint32_t ViewMatrixID = 0;
    uint32_t ModelMatrixID = 0;

    uint32_t TextureID = 0; 

    uint32_t LightID = 0;

    uint32_t Texture = 0;

};
