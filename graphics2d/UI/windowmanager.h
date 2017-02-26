#pragma once

struct IDevice;
class Camera;
class Scene;
class Manipulator;

struct WindowData
{
    double lastTime = 0.0f;
    std::shared_ptr<IDevice> spDevice;
    std::shared_ptr<Camera> spCamera;
    std::shared_ptr<Manipulator> spManipulator;
    bool mousePressed[3] = { false, false, false };
    float mouseWheel;
};

class WindowManager
{
public:
    WindowManager(std::shared_ptr<Scene>& spScene);
    void HandleEvents(bool& quit);

    void AddWindow(SDL_Window* pWindow, std::shared_ptr<IDevice> spDevice);
    void RemoveWindow(SDL_Window* pWindow);
    WindowData& GetWindowData(SDL_Window* pWindow);
    SDL_Window* GetWindowFromEvent(SDL_Event& e);

    void Update(SDL_Window* pWindow);
    glm::ivec4 GetSDLWindowRect(SDL_Window* pWindow);

    std::map<SDL_Window*, WindowData>& GetWindows() { return mapWindowToData; }
private:
    std::map<SDL_Window*, WindowData> mapWindowToData;
    std::shared_ptr<Scene>& spScene;
};