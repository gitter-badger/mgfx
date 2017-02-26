#pragma once

struct IDevice;
class Camera;
class Manipulator;

class Window
{
public:
    Window(std::shared_ptr<IDevice>& spDevice);

    const std::shared_ptr<IDevice>& GetDevice() const { return spDevice; }

    void SetCamera(std::shared_ptr<Camera>& camera) { spCamera = camera; }
    const std::shared_ptr<Camera>& GetCamera() const { return spCamera; }

    void SetManipulator(std::shared_ptr<Manipulator>& manip) { spManipulator = manip; }
    const std::shared_ptr<Manipulator>& GetManipulator() const { return spManipulator; }

    void SetUpdateTime(double time) { lastTime = time; }
    double GetUpdateTime() const { return lastTime; }

private:
    double lastTime = 0.0f;
    float mouseWheel = 0.0f;
    std::shared_ptr<IDevice> spDevice;
    std::shared_ptr<Camera> spCamera;
    std::shared_ptr<Manipulator> spManipulator;
    bool mousePressed[3] = { false, false, false };
};