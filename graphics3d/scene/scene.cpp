#include "common.h"
#include "scene.h"
#include "device/IDevice.h"
#include "camera/camera.h"

#include <common/fileutils.h>


Scene::Scene()
{
    m_vecCameras.push_back(std::make_shared<Camera>());
    SetCurrentCamera(m_vecCameras[0]);
}

void Scene::Render(IDevice* pDevice)
{
    for (auto& spMesh : m_vecMeshes)
    {
        pDevice->Draw(spMesh.get());
    }
}