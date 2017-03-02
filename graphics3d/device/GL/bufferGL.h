#pragma once

// A helper to draw dynamic data into a geoemtry buffer in opengl
// The idea is to gain some parallelism between writing buffer data and the hardware reading it.
// Really, this is just a simple wrapper around the GL API.
class BufferGL
{
public:
    BufferGL(DeviceGL* pDevice, int size, uint32_t bufferType = GL_ARRAY_BUFFER);
    ~BufferGL();

    // Return a pointer to a buffer, with num elements of size typesize free, and the offset 
    // where the buffer was mapped (for later rendering)
    void* Map(uint32_t num, uint32_t typeSize, uint32_t& offset);
    void UnMap();

    void Bind() const;
    void UnBind() const;
    void EnsureSize(uint32_t size);

    uint32_t GetBufferID() const { return m_bufferID; }
    uint32_t GetBufferType() const { return m_bufferType;  }

public:
    DeviceGL* m_pDevice = nullptr;
    uint32_t m_offset = 0;
    uint32_t m_bufferID = 0;
    uint32_t m_lastOffset = 0;
    uint32_t m_lastSize = 0;
    uint32_t m_size = 0;
    uint32_t m_bufferType = GL_ARRAY_BUFFER;
    bool m_mapped = false;
    bool m_reset = true;
};

