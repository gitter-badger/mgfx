#include "common.h"

#include <GL/deviceGL.h>
#include "app/renderUI.h"

#include "camera/camera.h"
#include "scene/scene.h"
#include "scene/mesh.h"
#include "ui/manipulator.h"

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create a simple scene
    // It contains a camera
    std::shared_ptr<Scene> spScene = std::make_shared<Scene>();
    spScene->SetClearColor(glm::vec4(0.7f, .7f, .8f, 1.0f));

    std::shared_ptr<Mesh> spMesh = std::make_shared<Mesh>();
    spMesh->Load("suzanne.obj");// sponza / sponza.obj");
    spScene->AddMesh(spMesh);

    auto pCamera = spScene->GetCurrentCamera();
    pCamera->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));

    // Create a manipulator to move the camera
    auto pManipulator = std::make_shared<Manipulator>(pCamera);

    // The 3D Device
    auto pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (!pDevice->Init(spScene))
    {
        return -1;
    }

    // The app's 2D UI
    RenderUI renderUI;
    if (!renderUI.Init(spScene))
    {
        return -1;
    }

    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            pDevice->ProcessEvent(event);
            if (event.type == SDL_QUIT)
            {
                done = true;
            }

            if (!ImGui::GetIO().WantCaptureMouse)
            {
                // Tell the manipulator any events it might need for moving the camera
                pManipulator->ProcessEvent(event);
            }
        }

        // Draw the3D content
        if (pDevice->PreRender())
        {
            // 3D Rendering of the scene
            pDevice->Render();

            // 2D UI
            pDevice->Prepare2D();
            renderUI.Render();

            // Display result
            pDevice->Swap();
        }
    }

    pDevice->Cleanup();

    SDL_Quit();

    return 0;
}
