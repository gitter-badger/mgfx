#include "common.h"
#include "ui/windowmanager.h"
#include "ui/window.h"
#include "IDevice.h"
#include "camera/camera.h"
#include "ui/manipulator.h"

WindowManager::WindowManager()
{

}

WindowManager& WindowManager::Instance()
{
    static WindowManager manager;
    return manager;
}

Window* WindowManager::GetWindow(SDL_Window* pWindow)
{
    auto itrFound = mapSDLToWindow.find(pWindow);
    if (itrFound != mapSDLToWindow.end())
    {
        return itrFound->second.get();
    }
    return nullptr;
}

SDL_Window* WindowManager::GetSDLWindow(Window* pWindow)
{
    auto itrFound = mapWindowToSDL.find(pWindow);
    if (itrFound != mapWindowToSDL.end())
    {
        return itrFound->second;
    }
    return nullptr;
}

Window* WindowManager::AddWindow(SDL_Window* pWindow, std::shared_ptr<IDevice> spDevice)
{
    auto spWindow = std::make_shared<Window>(spDevice);
    spWindow->SetCamera(std::make_shared<Camera>());
    spWindow->SetManipulator(std::make_shared<Manipulator>(spWindow->GetCamera()));
    mapSDLToWindow[pWindow] = spWindow;
    mapWindowToSDL[spWindow.get()] = pWindow;

    // Set initial window size
    int w, h;
    SDL_GetWindowSize(pWindow, &w, &h);
    spWindow->GetCamera()->SetFilmSize(glm::uvec2(w, h));

    return spWindow.get();
}

void WindowManager::RemoveWindow(Window* pWindow)
{
    mapWindowToSDL.erase(pWindow);
    for (auto& win : mapSDLToWindow)
    {
        if (win.second.get() == pWindow)
        {
            mapSDLToWindow.erase(win.first);
        }
    }
}

glm::ivec4 WindowManager::GetWindowRect(Window* pWindow)
{
    glm::ivec4 rect;
    SDL_Window* pSDLWindow = GetSDLWindow(pWindow);
    if (!pSDLWindow)
    {
        return glm::ivec4(0);
    }
    SDL_GetWindowSize(pSDLWindow, &rect.z, &rect.w);
    SDL_GetWindowPosition(pSDLWindow, &rect.x, &rect.y);
    return rect;
}

void WindowManager::Update(Window* pWindow)
{
    auto& io = ImGui::GetIO();
    auto pSDLWindow = GetSDLWindow(pWindow);
    if (!pSDLWindow)
    {
        return;
    }
        
    if (pWindow->GetManipulator())
    {
        pWindow->GetManipulator()->Update();
    }

    if (pWindow->GetCamera())
    {
        pWindow->GetCamera()->Update();
    }
}

SDL_Window* WindowManager::GetSDLWindowFromEvent(SDL_Event& e)
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
        for (auto sdlWindow : mapSDLToWindow)
        {
            auto rect = GetWindowRect(sdlWindow.second.get());
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

void WindowManager::HandleEvents(bool& quit)
{
    ImGuiIO& io = ImGui::GetIO();

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        auto pSDLWindow = GetSDLWindowFromEvent(e);
        auto pWindow = GetWindow(pSDLWindow);

        if (pWindow->GetDevice())
        {
            pWindow->GetDevice()->ProcessEvent(e);
        }

        if (!ImGui::GetIO().WantCaptureMouse)
        {
            // Tell the manipulator any events it might need for moving the camera
            if (pWindow->GetManipulator())
            {
                pWindow->GetManipulator()->ProcessEvent(e);
            }
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
                case SDL_WINDOWEVENT_RESIZED:
                    if (pWindow->GetCamera())
                    {
                        pWindow->GetCamera()->SetFilmSize(glm::uvec2(e.window.data1, e.window.data2));
                    }
                default:
                    break;
                }
            }
        }
        break;
        default:
            continue;
        }
    }
}
