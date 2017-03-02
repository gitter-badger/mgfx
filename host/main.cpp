
#include "common.h"

#include <GL/deviceGL.h>
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

void DrawWithCPU(Window* pWindow, uint32_t quadID)
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
                pixel = glm::u8vec4(x, x, x, 255);
            }
        }
    }

    // Use the graphics hardware to show our result
    pWindow->GetDevice()->UpdateQuad(quadID, bitmapData, size);
    pWindow->GetDevice()->DrawQuad(quadID, glm::vec4(0, 0, size.x, size.y));
}

// This is where we show our app GUI.
void ShowGUI(Window* pWindow,
    Scene* pScene)
{
    static bool show_test_window = false;

    int mode = int(AppSettings::Instance().GetMode());
    if (ImGui::Combo("Mode", &mode, "2D\0" "3D\0\0"))
    {
        AppSettings::Instance().SetMode(AppMode(mode));
        return;
    }

    if (AppSettings::Instance().GetMode() == AppMode::Display3D)
    {
        auto pCamera = pWindow->GetCamera();
        if (pCamera)
        {
            static float f = pCamera->GetFieldOfView();
            if (ImGui::SliderFloat("Field Of View", &f, 20.0f, 90.0f))
            {
                pCamera->SetFieldOfView(f);
            }
        }

        auto clear_color = pScene->GetClearColor();
        if (ImGui::ColorEdit3("clear color", (float*)&clear_color))
        {
            pScene->SetClearColor(clear_color);
        }
    }

    // For testing the GUI features
    if (ImGui::Button("UI Examples"))
    {
        show_test_window ^= 1;
    }

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }

    ImGui::Render();
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        LOG(ERROR) << SDL_GetError();
        return -1;
    }

    fs::path basePath = SDL_GetBasePath();
    el::Configurations conf((basePath / "logger.conf").string().c_str());
    el::Loggers::reconfigureAllLoggers(conf);

    auto spScene = LoadScene();

    uint32_t quad = 0;

    // OpenGL
    auto pDevice = std::static_pointer_cast<IDevice>(std::make_shared<DeviceGL>());
    if (pDevice->Init(spScene))
    {
        WindowManager::Instance().AddWindow(pDevice->GetSDLWindow(), pDevice);
        WindowManager::Instance().GetWindow(pDevice->GetSDLWindow())->GetCamera()->SetPositionAndFocalPoint(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f));

        quad = pDevice->CreateQuad();
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

                // 3D Rendering
                spWindow->GetDevice()->Begin3D();

                // Draw the scene
                spScene->Render(spWindow->GetDevice().get());

                spWindow->GetDevice()->End3D();
            }
            else if (AppSettings::Instance().GetMode() == AppMode::Display2D)
            {
                spWindow->GetDevice()->Begin2D();

                DrawWithCPU(spWindow.get(), quad);

                spWindow->GetDevice()->End2D();
            }

            // Finally, the GUI
            spWindow->GetDevice()->BeginGUI();

            ShowGUI(spWindow.get(), spScene.get());

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
