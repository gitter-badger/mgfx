#pragma once

struct IDevice;
class Camera;
class Scene;
class Manipulator;
class Window;

class WindowManager
{
public:
    WindowManager();

    void HandleEvents(bool& quit);

    Window* AddWindow(SDL_Window* pWindow, std::shared_ptr<IDevice> spDevice);
    void RemoveWindow(Window* pWindow);

    Window* GetWindow(SDL_Window* pWindow);

    void Update(Window* pWindow);
    glm::ivec4 GetWindowRect(Window* pWindow);

    std::map<SDL_Window*, std::shared_ptr<Window>>& GetWindows() { return mapSDLToWindow; }
private:
    SDL_Window* GetSDLWindow(Window* pWindow);
    SDL_Window* GetSDLWindowFromEvent(SDL_Event& e);
    std::map<SDL_Window*, std::shared_ptr<Window>> mapSDLToWindow;
    std::map<Window*, SDL_Window*> mapWindowToSDL;
};