#include "common.h"
#include "scene.h"
#include "device/IDevice.h"
#include "camera/camera.h"

#include <common/fileutils.h>


Scene::Scene()
{
}

void Scene::Render(IDevice* pDevice)
{
    for (auto& spMesh : m_vecMeshes)
    {
        pDevice->Draw(spMesh.get());
    }
}