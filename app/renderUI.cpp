#include "common.h"
#include "RenderUI.h"
#include "camera/camera.h"
#include "scene/scene.h"
#include "ui/windowmanager.h"
#include "ui/window.h"

bool RenderUI::Init(std::shared_ptr<Scene>& spScene)
{
    m_spScene = spScene;
    return true;
}

bool RenderUI::Render(Window* pWindow)
{
    if (!m_spScene)
    {
        return true;
    }

    static bool show_test_window = false;

    ImVec4 clear_color = m_spScene->GetClearColor();
    auto pCamera = pWindow->GetCamera();

    if (pCamera)
    {
        static float f = pCamera->GetFieldOfView();
        if (ImGui::SliderFloat("Field Of View", &f, 20.0f, 90.0f))
        {
            pCamera->SetFieldOfView(f);
        }
    }

    if (ImGui::ColorEdit3("clear color", (float*)&clear_color))
    {
        m_spScene->SetClearColor(clear_color);
    }
    
    if (ImGui::Button("Test Window")) show_test_window ^= 1;

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }

    ImGui::Render();

    return true;
}