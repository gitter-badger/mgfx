
#include "common.h"

#include <GL/deviceGL.h>
#include "app/renderUI.h"

#include "camera/camera.h"
#include "scene/scene.h"
#include "scene/mesh.h"
#include "ui/manipulator.h"
#include "ui/window.h"

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
    // It contains a camera
    std::shared_ptr<Scene> spScene = std::make_shared<Scene>();
    spScene->SetClearColor(glm::vec4(0.7f, .7f, .8f, 1.0f));

    std::shared_ptr<Mesh> spMesh = std::make_shared<Mesh>();
    spMesh->Load("models/sponza/sponza.obj");// sponza / sponza.obj");
    spScene->AddMesh(spMesh);

    auto pCamera = spScene->GetCurrentCamera();
    pCamera->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));

    // Create a manipulator to move the camera
    auto pManipulator = std::make_shared<Manipulator>(pCamera);

    // The app's 2D UI
    RenderUI renderUI;
    if (!renderUI.Init(spScene))
    {
        return -1;
    }

    Window windowManager;

    // OpenGL
    auto pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        windowManager.AddWindow(pDevice->GetWindow(), pDevice);
    }

    pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        windowManager.AddWindow(pDevice->GetWindow(), pDevice);
    }
    // Main loop
    bool done = false;
    while (!done)
    {
        windowManager.HandleEvents(done);

        /*if (!ImGui::GetIO().WantCaptureMouse)
        {
            // Tell the manipulator any events it might need for moving the camera
            pManipulator->ProcessEvent(event);
        }
        */

        pManipulator->Update();

        for (auto& window : windowManager.GetWindows())
        {
            windowManager.UpdateIMGUI(window.first);

            if (window.second.spDevice)
            {
                pCamera->Update(window.first);

                // 3D Rendering of the scene
                window.second.spDevice->Render();

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
