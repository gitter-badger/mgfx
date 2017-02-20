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