#pragma once

class BufferGL
{
public:
    BufferGL(DeviceGL* pDevice, int size);
    ~BufferGL();

    void* Map(uint32_t num, uint32_t typeSize, uint32_t& offset, bool reset = false);
    void UnMap();

    void Bind() const;
    void UnBind() const;
    void EnsureSize(uint32_t size);

    uint32_t GetBufferID() const { return m_bufferID; }
    uint32_t GetBufferType() const { return GL_ARRAY_BUFFER;  }

public:
    DeviceGL* m_pDevice = nullptr;
    uint32_t m_offset = 0;
    uint32_t m_bufferID = 0;
    uint32_t m_lastOffset = 0;
    uint32_t m_lastSize = 0;
    uint32_t m_size = 0;
    bool m_mapped = false;
};

