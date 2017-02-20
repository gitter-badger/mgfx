#include "common.h"
#include "scene.h"
#include "camera/camera.h"

Scene::Scene()
{
    m_vecCameras.push_back(std::make_shared<Camera>());
    SetCurrentCamera(m_vecCameras[0]);
}

