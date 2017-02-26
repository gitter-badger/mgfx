#pragma once

class Camera;
class Scene;
class Window;
struct WindowData;

class RenderUI
{
public:
    bool Init(std::shared_ptr<Scene>& spScene);
    bool Render(Window* pWindow);

private:
    std::shared_ptr<Scene> m_spScene;
};