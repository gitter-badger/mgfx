#pragma once

struct IDevice;

class Camera;
class Manipulator;

// A Window is a simple wrapper around an SDL_Window.
// It contains an active device, a camera that shows a view, and a manipulator that modifies it.
class Window
{
public:
    Window(std::shared_ptr<IDevice>& spDevice);

    void PreRender();

    const std::shared_ptr<IDevice>& GetDevice() const { return m_spDevice; }

    void SetCamera(std::shared_ptr<Camera>& camera) { m_spCamera = camera; }
    const std::shared_ptr<Camera>& GetCamera() const { return m_spCamera; }

    void SetManipulator(std::shared_ptr<Manipulator>& manip) { m_spManipulator = manip; }
    const std::shared_ptr<Manipulator>& GetManipulator() const { return m_spManipulator; }

    void SetUpdateTime(double time) { m_lastTime = time; }
    double GetUpdateTime() const { return m_lastTime; }

private:
    double m_lastTime = 0.0f;
    float m_mouseWheel = 0.0f;
    std::shared_ptr<IDevice> m_spDevice;
    std::shared_ptr<Camera> m_spCamera;
    std::shared_ptr<Manipulator> m_spManipulator;
    bool m_mousePressed[3] = { false, false, false };
};