#include "common.h"
#include "deviceGL.h"
#include "bufferGL.h"

BufferGL::BufferGL(DeviceGL* pDevice, int size )
    : m_pDevice(pDevice),
    m_size(size)
{
    CHECK_GL(glGenBuffers(1, &m_bufferID));

    Bind();
    
    CHECK_GL(glBufferData(GetBufferType(), size, NULL, GL_DYNAMIC_DRAW));
}

BufferGL::~BufferGL()
{
    CHECK_GL(glDeleteBuffers(1, &m_bufferID));
}

void BufferGL::Bind() const
{
    CHECK_GL(glBindBuffer(GetBufferType(), m_bufferID));
}

void BufferGL::UnBind() const
{
    CHECK_GL(glBindBuffer(GetBufferType(), 0));
}

void BufferGL::EnsureSize(uint32_t size)
{
    // Force it to grow
    if (size > m_size)
    {
        CHECK_GL(glBufferData(GetBufferType(), size, NULL, GL_DYNAMIC_DRAW));
        m_size = size;
        m_offset = 0;
    }
}

void* BufferGL::Map(unsigned int num, unsigned int typeSize, unsigned int& offset, bool reset)
{
    assert(!m_mapped);
    if (m_mapped)
    {
        return nullptr;
    }

    if (m_bufferID == 0)
    {
        return nullptr;
    }

    m_mapped = true;

    Bind();

    uint32_t byteSize = num * typeSize;
    if (reset || ((m_offset + byteSize) > m_size))
    {
        offset = 0;
        m_offset = byteSize;

        return glMapBufferRange(GetBufferType(), 0, byteSize, GL_WRITE_ONLY | GL_MAP_INVALIDATE_BUFFER_BIT);
    }
    else
    {
        // Return the last offset we were up to
        offset = m_offset;
        m_offset = m_offset + byteSize;

        return glMapBufferRange(GetBufferType(), offset, byteSize, GL_MAP_WRITE_BIT /*| GL_MAP_INVALIDATE_RANGE_BIT*/ | GL_MAP_UNSYNCHRONIZED_BIT);
    }
}

void BufferGL::UnMap()
{
    assert(m_mapped);
    if (m_mapped)
    {
        Bind();
        CHECK_GL(glUnmapBuffer(GetBufferType()));
        m_mapped = false;
    }
}

