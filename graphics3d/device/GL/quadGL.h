#pragma once

struct QuadData
{
    uint32_t ImageID = 0;
    uint32_t PBOBufferID = 0;
    uint32_t currentPBO = 0;
    glm::uvec2 Size = glm::uvec2(0);
};

class BufferGL;
class QuadGL
{
public:
    QuadGL(DeviceGL* pDevice);
    ~QuadGL();

    uint32_t CreateQuad();
    void DestroyQuad(uint32_t id);
    void UpdateQuad(uint32_t id, const std::vector<glm::u8vec4>& quadData, const glm::uvec2& size);
    void DrawQuad(uint32_t id, const glm::vec4& target);
    void BeginDraw(uint32_t maxQuads);
    void EndDraw();

private:
    DeviceGL* m_pDevice = nullptr;
    std::map<uint32_t, std::shared_ptr<QuadData>> m_mapQuadData;
    std::shared_ptr<BufferGL> m_spPositionBuffer;
    std::shared_ptr<BufferGL> m_spColorBuffer;
    std::shared_ptr<BufferGL> m_spTexCoordBuffer;
    std::shared_ptr<BufferGL> m_spIndexBuffer;
    uint32_t VertexArrayID = 0;
    uint32_t m_programID = 0;
    uint32_t m_samplerID = 0;
    uint32_t m_projectionID = 0;

    uint32_t posOffset = 0;
    uint32_t colorOffset = 0;
    uint32_t texCoordOffset = 0;
    uint32_t indexOffset = 0;
    glm::vec4 lastTarget = glm::vec4(0.0f);
};

