#pragma once

struct IDevice;

struct WindowData
{
    double lastTime = 0.0f;
    std::shared_ptr<IDevice> spDevice;
    bool mousePressed[3] = { false, false, false };
    float mouseWheel;
};

class Window
{
public:
    void HandleEvents(bool& quit);

    void AddWindow(SDL_Window* pWindow, std::shared_ptr<IDevice> spDevice);
    void RemoveWindow(SDL_Window* pWindow);
    WindowData& GetWindowData(SDL_Window* pWindow);
    SDL_Window* GetWindowFromEvent(SDL_Event& e);

    void UpdateIMGUI(SDL_Window* pWindow);
    glm::ivec4 GetSDLWindowRect(SDL_Window* pWindow);

    const std::map<SDL_Window*, WindowData>& GetWindows() const { return mapWindowToData; }
private:
    std::map<SDL_Window*, WindowData> mapWindowToData;
};