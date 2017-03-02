
#include "common.h"

#include <GL/deviceGL.h>
#include "app/renderUI.h"
#include "app/settings.h"

#include "camera/camera.h"
#include "scene/scene.h"
#include "geometry/mesh.h"
#include "ui/manipulator.h"
#include "ui/windowmanager.h"
#include "ui/window.h"

INITIALIZE_EASYLOGGINGPP

std::shared_ptr<Scene> LoadScene()
{
    // Create a simple scene
    auto spScene = std::make_shared<Scene>();
    spScene->SetClearColor(glm::vec4(0.7f, .7f, .8f, 1.0f));

    auto meshPath = GetMediaPath("models/sponza/sponza.obj");
    if (!meshPath.empty())
    {
        auto spMesh = std::make_shared<Mesh>();
        spMesh->Load(meshPath);
        spScene->AddMesh(spMesh);
    }

    return spScene;
}

void DrawWindowImage(Window* pWindow)
{
    auto size = pWindow->GetClientRect();

    static std::vector<glm::u8vec4> bitmapData;
    bitmapData.resize(size.x * size.y);

    auto PixelData = [&](int x, int y) { return bitmapData[y * size.x + x]; };

    static int currentX = 0;
    currentX++;
    currentX = currentX % size.x;

    for (uint32_t y = 0; y < size.y; y++)
    {
        for (uint32_t x = 0; x < size.x; x++)
        {
            auto& pixel = bitmapData[y * size.x + x];
            if (currentX == x)
            {
                pixel = glm::u8vec4(255);
            }
            else
            {
                pixel = glm::u8vec4(25);
            }
        }
    }

    pWindow->GetDevice()->Draw2D(bitmapData, size);
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        LOG(ERROR) << SDL_GetError();
        return -1;
    }

    auto spScene = LoadScene();

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
        WindowManager::Instance().AddWindow(pDevice->GetSDLWindow(), pDevice);
        WindowManager::Instance().GetWindow(pDevice->GetSDLWindow())->GetCamera()->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));
    }
    pDevice = nullptr;

    // Main loop
    bool done = false;
    while (!done)
    {
        // Have the window manager check all the events for the windows.
        WindowManager::Instance().HandleEvents(done);

        // Walk the list of windows currently drawing
        for (auto& windows : WindowManager::Instance().GetWindows())
        {
            auto& spWindow = windows.second;

            // Setup window
            spWindow->PreRender();

            if (AppSettings::Instance().GetMode() == AppMode::Display3D)
            {
                // Set the current camera
                spScene->SetCurrentCamera(spWindow->GetCamera().get());

                // 3D Rendering prep
                spWindow->GetDevice()->Prepare3D();

                // Draw the scene
                spScene->Render(spWindow->GetDevice().get());
            }
            else if (AppSettings::Instance().GetMode() == AppMode::Display2D)
            {
                spWindow->GetDevice()->Prepare2D();

                DrawWindowImage(spWindow.get());
            }

            // 2D Rendering prep
            spWindow->GetDevice()->BeginGUI();

            // Draw the 2D
            renderUI.Render(spWindow.get());

            spWindow->GetDevice()->EndGUI();

            // Display result
            spWindow->GetDevice()->Swap();
        }
    }

    // We are done, cleanup
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
