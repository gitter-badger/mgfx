#include "common.h"
#include "deviceGL.h"
#include "quadGL.h"
#include "bufferGL.h"

QuadGL::QuadGL(DeviceGL* pDevice)
    : m_pDevice(pDevice)
{
    CHECK_GL(glGenVertexArrays(1, &VertexArrayID));
    CHECK_GL(glBindVertexArray(VertexArrayID));

    m_spPositionBuffer = std::make_shared<BufferGL>(pDevice, 1000);
    m_spColorBuffer = std::make_shared<BufferGL>(pDevice, 1000);
    m_spTexCoordBuffer = std::make_shared<BufferGL>(pDevice, 1000);
    
    CHECK_GL(glBindVertexArray(0));
    
    // Create and compile our GLSL program from the shaders
    m_programID = LoadShaders(GetMediaPath("shaders/GL/Quad.vertexshader").c_str(), GetMediaPath("shaders/GL/Quad.fragmentshader").c_str());
    
    glUseProgram(m_programID);
    m_samplerID = glGetUniformLocation(m_programID, "albedo_sampler");
    glUniform1i(m_samplerID, 0);
    glUseProgram(0);
}

QuadGL::~QuadGL()
{
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(m_programID);
    for (auto& qd : m_mapQuadData)
    {
        glDeleteTextures(1, &qd.first);
    }
    m_mapQuadData.clear();
}

uint32_t QuadGL::CreateQuad()
{
    uint32_t tex;
    CHECK_GL(glGenTextures(1, &tex));

    m_mapQuadData[tex] = std::make_shared<QuadData>();
    return tex;
}

void QuadGL::DestroyQuad(uint32_t id)
{
    auto itr = m_mapQuadData.find(id);
    if (itr == m_mapQuadData.end())
    {
        return;
    }
    m_mapQuadData.erase(itr);

    CHECK_GL(glDeleteTextures(1, &id));
}

void QuadGL::UpdateQuad(uint32_t id, const std::vector<glm::u8vec4>& quadData, const glm::uvec2& size)
{
    auto itr = m_mapQuadData.find(id);
    if (itr == m_mapQuadData.end())
    {
        return;
    }
    itr->second->Size = size;
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &quadData[0]));
}

void QuadGL::BeginDraw(uint32_t maxQuads)
{
    CHECK_GL(glBindVertexArray(VertexArrayID));
    CHECK_GL(glDisable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glActiveTexture(GL_TEXTURE0));
    CHECK_GL(glUseProgram(m_programID));
    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glEnableVertexAttribArray(1));
    CHECK_GL(glEnableVertexAttribArray(2));
}

void QuadGL::EndDraw()
{
    CHECK_GL(glBindVertexArray(0));
    CHECK_GL(glUseProgram(0));
    CHECK_GL(glDisableVertexAttribArray(0));
    CHECK_GL(glDisableVertexAttribArray(1));
    CHECK_GL(glDisableVertexAttribArray(2));
}

void QuadGL::DrawQuad(uint32_t id, const glm::vec4& target)
{
    auto itr = m_mapQuadData.find(id);
    if (itr == m_mapQuadData.end())
    {
        return;
    }

    uint32_t posOffset;
    uint32_t colorOffset;
    uint32_t texCoordOffset;
    auto pPos = (glm::uvec2*)m_spPositionBuffer->Map(4, sizeof(glm::uvec2), posOffset, false);
    auto pColor = (glm::vec4*)m_spColorBuffer->Map(4, sizeof(glm::vec4), colorOffset, false);
    auto pTexCoord = (glm::vec2*)m_spTexCoordBuffer->Map(4, sizeof(glm::vec2), texCoordOffset, false);

    pPos[0] = glm::uvec2(target.x, target.y);
    pPos[1] = glm::uvec2(target.x + target.z, target.y);
    pPos[2] = glm::uvec2(target.x + target.z, target.y + target.z);
    pPos[3] = glm::uvec2(target.x, target.y + target.z);

    pTexCoord[0] = glm::vec2(0.0f, 1.0f);
    pTexCoord[1] = glm::vec2(1.0f, 1.0f);
    pTexCoord[2] = glm::vec2(1.0f, 0.0f);
    pTexCoord[3] = glm::vec2(0.0f, 0.0f);

    pColor[0] = glm::vec4(1.0f);
    pColor[1] = glm::vec4(1.0f);
    pColor[2] = glm::vec4(1.0f);
    pColor[3] = glm::vec4(1.0f);

    m_spPositionBuffer->UnMap();
    m_spColorBuffer->UnMap();
    m_spTexCoordBuffer->UnMap();

    CHECK_GL(glBindTexture(GL_TEXTURE_2D, id));

    m_spPositionBuffer->Bind();
    CHECK_GL(glVertexAttribPointer(0, 2, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)(uintptr_t)posOffset));

    m_spTexCoordBuffer->Bind();
    CHECK_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)texCoordOffset));

    m_spColorBuffer->Bind();
    CHECK_GL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)colorOffset));

    CHECK_GL(glDrawArrays(GL_QUADS, 0, 1));
}


