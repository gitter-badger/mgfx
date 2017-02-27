#include "Common.h"
#include "ui/window.h"
#include "ui/manipulator.h"
#include "camera/camera.h"

Window::Window(std::shared_ptr<IDevice>& spDevice)
    : m_spDevice(spDevice)
{

}

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
