
#include "common.h"

#include <GL/deviceGL.h>
#include "app/renderUI.h"

#include "camera/camera.h"
#include "scene/scene.h"
#include "scene/mesh.h"
#include "ui/manipulator.h"
#include "ui/windowmanager.h"

INITIALIZE_EASYLOGGINGPP

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) 
    {
        LOG(ERROR) << SDL_GetError();
        return -1;
    }


    // Create a simple scene
    auto spScene = std::make_shared<Scene>();
    spScene->SetClearColor(glm::vec4(0.7f, .7f, .8f, 1.0f));

    auto spMesh = std::make_shared<Mesh>();
    spMesh->Load("models/sponza/sponza.obj");// sponza / sponza.obj");
    spScene->AddMesh(spMesh);

    // The app's 2D UI
    RenderUI renderUI;
    if (!renderUI.Init(spScene))
    {
        return -1;
    }

    WindowManager windowManager(spScene);

    // OpenGL
    auto pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        windowManager.AddWindow(pDevice->GetWindow(), pDevice);
        windowManager.GetWindowData(pDevice->GetWindow()).spCamera->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
    }

    pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        windowManager.AddWindow(pDevice->GetWindow(), pDevice);
        windowManager.GetWindowData(pDevice->GetWindow()).spCamera->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
    }

    // Main loop
    bool done = false;
    while (!done)
    {
        windowManager.HandleEvents(done);

        for (auto& window : windowManager.GetWindows())
        {
            windowManager.Update(window.first);

            if (window.second.spDevice)
            {
                // 3D Rendering of the scene
                window.second.spDevice->Render(window.second);

                // 2D UI
                //window.second.spDevice->Prepare2D();
                //renderUI.Render();

                // Display result
                window.second.spDevice->Swap();
            }
        }
    }


    for (auto& window : windowManager.GetWindows())
    {
        if (window.second.spDevice)
        {
            window.second.spDevice->Cleanup();
        }
    }

    SDL_Quit();

    return 0;
}
