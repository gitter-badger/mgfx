#pragma once

class Camera;
class Scene;
class RenderUI
{
public:
    bool Init(std::shared_ptr<Scene>& spScene);
    bool Render();

private:
    std::shared_ptr<Scene> m_spScene;
};