
#include "common.h"

#include <GL/deviceGL.h>
#include "app/renderUI.h"

#include "camera/camera.h"
#include "scene/scene.h"
#include "scene/mesh.h"
#include "ui/manipulator.h"
#include "ui/windowmanager.h"
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

    // OpenGL
    auto pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        WindowManager::Instance().AddWindow(pDevice->GetWindow(), pDevice);
        WindowManager::Instance().GetWindow(pDevice->GetWindow())->GetCamera()->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
    }
    pDevice = nullptr;

    pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        WindowManager::Instance().AddWindow(pDevice->GetWindow(), pDevice);
        WindowManager::Instance().GetWindow(pDevice->GetWindow())->GetCamera()->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
    }
    pDevice = nullptr;

    // Main loop
    bool done = false;
    while (!done)
    {
        WindowManager::Instance().HandleEvents(done);

        for (auto& window : WindowManager::Instance().GetWindows())
        {
            WindowManager::Instance().Update(window.second.get());

            if (window.second->GetDevice())
            {
                spScene->SetCurrentCamera(window.second->GetCamera().get());

                // 3D Rendering of the scene
                window.second->GetDevice()->Render();

                // 2D UI
                window.second->GetDevice()->Prepare2D();
                renderUI.Render(window.second.get());
                window.second->GetDevice()->Render2D();

                // Display result
                window.second->GetDevice()->Swap();
            }
        }
    }


    for (auto& window : WindowManager::Instance().GetWindows())
    {
        if (window.second->GetDevice())
        {
            window.second->GetDevice()->Cleanup();
        }
    }

    ImGui::Shutdown();
    SDL_Quit();

    return 0;
}
