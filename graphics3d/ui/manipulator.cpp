#include "common.h"
#include "manipulator.h"

void Manipulator::ProcessEvent(SDL_Event& event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        /* If the left button was pressed. */
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            MouseDown(glm::vec2(x, y));
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP)
    {
        /* If the left button was pressed. */
        if (event.button.button == SDL_BUTTON_LEFT) 
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            MouseUp(glm::vec2(x, y));
        }
    }
    else if (event.type == SDL_MOUSEMOTION)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        MouseMove(glm::vec2(x, y));
    }
    else if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            spCamera->SetFilmSize(glm::uvec2(event.window.data1, event.window.data2));
        }
    }
    else if (event.type == SDL_KEYDOWN ||
        event.type == SDL_KEYUP)
    {
        ProcessKeyboard(event);
    }
}

void Manipulator::ProcessKeyboard(SDL_Event& event)
{
    auto io = ImGui::GetIO();
    auto keyState = SDL_GetKeyboardState(nullptr);
   
    if (ImGui::GetIO().WantCaptureKeyboard ||
        ImGui::GetIO().WantTextInput)
    {
        walkDirection = glm::vec3(0.0f);
        return;
    }

    auto key = event.key.keysym.scancode;
    if (key == SDL_SCANCODE_W)
    {
        walkDirection.z = event.key.state == SDL_PRESSED ? 1.0f : 0.0f;
    }
    else if (key == SDL_SCANCODE_S)
    {
        walkDirection.z = event.key.state == SDL_PRESSED ? -1.0f : 0.0f;
    }
    else if (key == SDL_SCANCODE_A)
    {
        walkDirection.x = event.key.state == SDL_PRESSED ? -1.0f : 0.0f;
    }
    else if (key == SDL_SCANCODE_D)
    {
        walkDirection.x = event.key.state == SDL_PRESSED ? 1.0f : 0.0f;
    }
    else if (key == SDL_SCANCODE_R)
    {
        walkDirection.y = event.key.state == SDL_PRESSED ? 1.0f : 0.0f;
    }
    else if (key == SDL_SCANCODE_F)
    {
        walkDirection.y = event.key.state == SDL_PRESSED ? -1.0f : 0.0f;
    }
}

bool Manipulator::MouseMove(const glm::vec2& pos)
{
    currentPos = pos;
    auto keyState = SDL_GetKeyboardState(nullptr);
    if (mouseDown)
    {
        if (keyState[SDL_SCANCODE_LCTRL])
        {
            spCamera->Dolly((startPos.y - currentPos.y) / 4.0f);
        }
        else
        {
            spCamera->Orbit((currentPos - startPos) / 2.0f);
        }
        startPos = pos;
    }
    return true;
}

void Manipulator::Update()
{
    if (walkDirection != glm::vec3(0.0f))
    {
        spCamera->Walk(walkDirection);
    }
}