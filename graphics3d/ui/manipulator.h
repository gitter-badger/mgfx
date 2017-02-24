#pragma once
#include "camera/camera.h"

class Manipulator
{
private:
    glm::vec2 startPos;
    glm::vec2 currentPos;
    glm::vec3 walkDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    std::shared_ptr<Camera> spCamera;
    bool mouseDown;

public:
    Manipulator(std::shared_ptr<Camera> pCam)
        : spCamera(pCam),
        mouseDown(false)
    {

    }

    void MouseDown(const glm::vec2& pos)
    {
        startPos = pos;
        mouseDown = true;
    }

    void MouseUp(const glm::vec2& pos)
    {
        currentPos = pos;
        mouseDown = false;
    }

    bool MouseMove(const glm::vec2& pos);

    void ProcessEvent(SDL_Event& ev);
    void ProcessKeyboard(SDL_Event& ev);

    void Update();
};