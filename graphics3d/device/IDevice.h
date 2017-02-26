#pragma once

class Scene;
class Mesh;
struct WindowData;

struct IDevice
{
    virtual bool Init(std::shared_ptr<Scene>& spScene) = 0;
    virtual bool Render(WindowData& window) = 0;
    virtual void Prepare2D() = 0;
    virtual void Cleanup() = 0;
    virtual void ProcessEvent(SDL_Event& event) = 0;
    virtual void Swap() = 0;

    virtual void Draw(Mesh* pMesh) = 0;

    virtual SDL_Window* GetWindow() const = 0;
};