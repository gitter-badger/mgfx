#include "common.h"
#include "window.h"
#include "IDevice.h"

WindowData& Window::GetWindowData(SDL_Window* pWindow)
{
    return mapWindowToData[pWindow];
}

void Window::AddWindow(SDL_Window* pWindow, std::shared_ptr<IDevice> spDevice)
{
    WindowData data;
    data.spDevice = spDevice;
    data.lastTime = 0;
    mapWindowToData[pWindow] = data;
}

void Window::RemoveWindow(SDL_Window* pWindow)
{
    mapWindowToData.erase(pWindow);
}

glm::ivec4 Window::GetSDLWindowRect(SDL_Window* pWindow)
{
    glm::ivec4 rect;
    SDL_GetWindowSize(pWindow, &rect.z, &rect.w);
    SDL_GetWindowPosition(pWindow, &rect.x, &rect.y);
    return rect;
}

void Window::UpdateIMGUI(SDL_Window* pWindow)
{
    auto& io = ImGui::GetIO();
    auto& windowData = GetWindowData(pWindow);
    
    glm::ivec2 size;
    SDL_GetWindowSize(pWindow, &size.x, &size.y);

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(float(size.x), float(size.y));

    // Setup time step
    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = windowData.lastTime > 0.0 ? (float)(current_time - windowData.lastTime) : 0.0f;
    windowData.lastTime = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(pWindow) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    else
        io.MousePos = ImVec2(-1, -1);

    io.MouseDown[0] = windowData.mousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = windowData.mousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = windowData.mousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    windowData.mousePressed[0] = windowData.mousePressed[1] = windowData.mousePressed[2] = false;

    io.MouseWheel = windowData.mouseWheel;
    windowData.mouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);
}

SDL_Window* Window::GetWindowFromEvent(SDL_Event& e)
{
    switch (e.type)
    {
    case SDL_WINDOWEVENT:
        return SDL_GetWindowFromID(e.window.windowID);
    case SDL_MOUSEWHEEL:
        return SDL_GetWindowFromID(e.wheel.windowID);
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        return SDL_GetWindowFromID(e.button.windowID);
    case SDL_MOUSEMOTION:
        return SDL_GetWindowFromID(e.motion.windowID);
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        return SDL_GetWindowFromID(e.key.windowID);
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
    case SDL_FINGERMOTION:
    {
        for (auto sdlWindow : mapWindowToData)
        {
            auto rect = GetSDLWindowRect(sdlWindow.first);
            auto pos = glm::ivec2(e.tfinger.x, e.tfinger.y);
            pos.x *= rect.z;
            pos.y *= rect.w;
            if (RectContains(rect, pos))
            {
                return sdlWindow.first;
            }
        }
    }
    break;
    default:
        break;
    }

    // Last ditch
    return SDL_GL_GetCurrentWindow();
}

void Window::HandleEvents(bool& quit)
{
    ImGuiIO& io = ImGui::GetIO();

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        auto pWindow = GetWindowFromEvent(e);
        auto& windowData = GetWindowData(pWindow);

        if (windowData.spDevice)
        {
            windowData.spDevice->ProcessEvent(e);
        }

        if (e.type == SDL_QUIT)
        {
            quit = true;
            break;
        }

        switch (e.type)
        {
        case SDL_WINDOWEVENT:
        {
            if (pWindow)
            {
                switch (e.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    if (pWindow)
                    {
                        RemoveWindow(pWindow);
                    }
                    break;
                default:
                    break;
                }
            }
        }
        break;

        case SDL_MOUSEWHEEL:
        {
            if (e.wheel.y > 0)
                windowData.mouseWheel = 1;
            if (e.wheel.y < 0)
                windowData.mouseWheel = -1;
        }
        break;

        case SDL_KEYDOWN:
        {
            int key = e.key.keysym.sym & ~SDLK_SCANCODE_MASK;
            io.KeysDown[key] = (e.type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        }
        break;

        case SDL_KEYUP:
        {
            int key = e.key.keysym.sym & ~SDLK_SCANCODE_MASK;
            io.KeysDown[key] = (e.type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        }
        break;

        case SDL_TEXTINPUT:
        {
            io.AddInputCharactersUTF8(e.text.text);
        }
        break;

        case SDL_QUIT:
            quit = true;
            break;
        default:
            continue;
        }
    }
}
