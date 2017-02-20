#pragma once

class Scene;
struct IDevice
{
    virtual bool Init(std::shared_ptr<Scene>& spScene) = 0;
    virtual bool PreRender() = 0;
    virtual bool Render() = 0;
    virtual void Prepare2D() = 0;
    virtual void Cleanup() = 0;
    virtual void ProcessEvent(SDL_Event& event) = 0;
    virtual void Swap() = 0;

    virtual SDL_Window* GetWindow() const = 0;
};