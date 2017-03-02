#include "Common.h"
#include "ui/window.h"
#include "ui/manipulator.h"
#include "graphics3d/device/IDevice.h"
#include "camera/camera.h"

Window::Window(std::shared_ptr<IDevice>& spDevice)
    : m_spDevice(spDevice)
{

}

// Update the camera and manipulator owned by the window
void Window::PreRender()
{
    if (m_spManipulator)
    {
        m_spManipulator->Update();
    }

    if (m_spCamera)
    {
        m_spCamera->Update();
    }
}

glm::uvec2 Window::GetClientRect() const
{
    auto pSDLWindow = m_spDevice->GetSDLWindow();
    int w, h;
    SDL_GetWindowSize(pSDLWindow, &w, &h);
    return glm::uvec2(w, h);
}
