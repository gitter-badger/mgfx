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
    m_spIndexBuffer = std::make_shared<BufferGL>(pDevice, 4000, GL_ELEMENT_ARRAY_BUFFER);
    
    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glEnableVertexAttribArray(1));
    CHECK_GL(glEnableVertexAttribArray(2));

    CHECK_GL(glBindVertexArray(0));
    
    // Create and compile our GLSL program from the shaders
    m_programID = LoadShaders(GetMediaPath("shaders/GL/Quad.vertexshader").c_str(), GetMediaPath("shaders/GL/Quad.fragmentshader").c_str());
    
    glUseProgram(m_programID);
    glActiveTexture(GL_TEXTURE0);
    m_samplerID = glGetUniformLocation(m_programID, "albedo_sampler");
    m_projectionID = glGetUniformLocation(m_programID, "Projection");
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
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, tex));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));

    auto spQuadData = std::make_shared<QuadData>();
    CHECK_GL(glGenBuffers(1, &spQuadData->PBOBufferID));
    spQuadData->currentPBO = 0;

    m_mapQuadData[tex] = spQuadData;
    return tex;
}

void QuadGL::DestroyQuad(uint32_t id)
{
    auto itr = m_mapQuadData.find(id);
    if (itr == m_mapQuadData.end())
    {
        return;
    }
    glDeleteBuffers(1, &itr->second->PBOBufferID);
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

    auto byteSize = size.x * size.y * sizeof(glm::u8vec4);
    auto& spQuad = itr->second;
    bool skip = false;
    if (spQuad->Size != size)
    {
        CHECK_GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, spQuad->PBOBufferID));
        CHECK_GL(glBufferData(GL_PIXEL_UNPACK_BUFFER, byteSize, 0, GL_STREAM_DRAW));
        itr->second->Size = size;
        skip = true;
    }

    CHECK_GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, spQuad->PBOBufferID));
    GLubyte* ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, byteSize, GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
    memcpy(ptr, &quadData[0], byteSize);
    CHECK_GL(glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER));

    CHECK_GL(glBindTexture(GL_TEXTURE_2D, id));
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    CHECK_GL(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
}

void QuadGL::BeginDraw(uint32_t maxQuads)
{
    CHECK_GL(glDisable(GL_CULL_FACE));
    CHECK_GL(glDisable(GL_DEPTH_TEST));
    CHECK_GL(glEnable(GL_BLEND));
    CHECK_GL(glUseProgram(m_programID));
    CHECK_GL(glBindVertexArray(VertexArrayID));
    CHECK_GL(glActiveTexture(GL_TEXTURE0));
    
    auto& io = ImGui::GetIO();
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUniformMatrix4fv(m_projectionID, 1, GL_FALSE, &ortho_projection[0][0]);
}

void QuadGL::EndDraw()
{
    CHECK_GL(glUseProgram(0));
    CHECK_GL(glEnable(GL_CULL_FACE));
    CHECK_GL(glEnable(GL_DEPTH_TEST));
}

void QuadGL::DrawQuad(uint32_t id, const glm::vec4& target)
{
    auto itr = m_mapQuadData.find(id);
    if (itr == m_mapQuadData.end() || target == glm::vec4(0.0f))
    {
        return;
    }

    // Update the geometry if different
    // Saves time if all we are doing is drawing full screen quads
    if (lastTarget != target)
    {
        lastTarget = target;

        auto pPos = (glm::vec2*)m_spPositionBuffer->Map(4, sizeof(glm::vec2), posOffset);
        auto pColor = (glm::vec4*)m_spColorBuffer->Map(4, sizeof(glm::vec4), colorOffset);
        auto pTexCoord = (glm::vec2*)m_spTexCoordBuffer->Map(4, sizeof(glm::vec2), texCoordOffset);
        auto pIndices = (uint32_t*)m_spIndexBuffer->Map(6, sizeof(uint32_t), indexOffset);

        pPos[0] = glm::vec2(target.x, target.y);
        pPos[1] = glm::vec2(target.x + target.z, target.y);
        pPos[2] = glm::vec2(target.x + target.z, target.y + target.w);
        pPos[3] = glm::vec2(target.x, target.y + target.w);

        pTexCoord[0] = glm::vec2(0.0f, 1.0f);
        pTexCoord[1] = glm::vec2(1.0f, 1.0f);
        pTexCoord[2] = glm::vec2(1.0f, 0.0f);
        pTexCoord[3] = glm::vec2(0.0f, 0.0f);

        pColor[0] = glm::vec4(1.0f);
        pColor[1] = glm::vec4(1.0f);
        pColor[2] = glm::vec4(1.0f);
        pColor[3] = glm::vec4(1.0f);

        pIndices[0] = 0;
        pIndices[1] = 1;
        pIndices[2] = 2;
        pIndices[3] = 2;
        pIndices[4] = 3;
        pIndices[5] = 0;

        m_spPositionBuffer->UnMap();
        m_spColorBuffer->UnMap();
        m_spTexCoordBuffer->UnMap();
        m_spIndexBuffer->UnMap();

        CHECK_GL(glActiveTexture(GL_TEXTURE0));
        CHECK_GL(glBindTexture(GL_TEXTURE_2D, id));

        m_spPositionBuffer->Bind();
        CHECK_GL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)posOffset));

        m_spTexCoordBuffer->Bind();
        CHECK_GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)texCoordOffset));

        m_spColorBuffer->Bind();
        CHECK_GL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)(uintptr_t)colorOffset));
    }

    m_spIndexBuffer->Bind();
    CHECK_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(uintptr_t)indexOffset));
}


