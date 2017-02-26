#pragma once

class Camera;
class Scene;
struct WindowData;

class RenderUI
{
public:
    bool Init(std::shared_ptr<Scene>& spScene);
    bool Render(WindowData& window);

private:
    std::shared_ptr<Scene> m_spScene;
};