#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Camera.h"

class GLRender
{
public:
    GLRender();
    bool Init();
    bool Render(Camera* pCamera);
    void Cleanup();

private:
    uint32_t VertexArrayID = 0;

    uint32_t programID = 0;

    uint32_t MatrixID = 0;
    uint32_t ViewMatrixID = 0;
    uint32_t ModelMatrixID = 0;

    uint32_t TextureID = 0; 

    uint32_t LightID = 0;

    uint32_t Texture = 0;

    uint32_t vertexbuffer = 0;
    uint32_t uvbuffer = 0;
    uint32_t normalbuffer = 0;

    uint32_t numVertices = 0;
};
