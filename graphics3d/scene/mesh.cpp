#include "common.h"
#include "fileutils.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "mesh.h"

bool Mesh::Load(const char* pszModel)
{
    auto strPath = GetMediaPath(pszModel);
    if (strPath.empty())
    {
        return false;
    }

    std::string err;
    bool ret = tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &err, strPath.c_str());

    if (!err.empty())
    { // `err` may contain warning message.
        return false;
    }

    if (!ret)
    {
        exit(1);
    }

    /*
    // Loop over shapes
    for (size_t s = 0; s < m_shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < m_shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int fv = m_shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = m_shapes[s].mesh.indices[index_offset + v];
                float vx = m_attrib.vertices[3 * idx.vertex_index + 0];
                float vy = m_attrib.vertices[3 * idx.vertex_index + 1];
                float vz = attrib.vertices[3 * idx.vertex_index + 2];
                float nx = attrib.normals[3 * idx.normal_index + 0];
                float ny = attrib.normals[3 * idx.normal_index + 1];
                float nz = attrib.normals[3 * idx.normal_index + 2];
                float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
    */
    return true;
}
