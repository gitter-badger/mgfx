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
    {
        return false;
    }

    std::shared_ptr<MeshPart> spPart;
    for (auto& shape : m_shapes)
    {
        tinyobj::mesh_t& m = shape.mesh;

        bool newPart = true;
        for (size_t j = 0; j < m.indices.size(); j += 3)
        {
            if (spPart &&
                spPart->MaterialID != m.material_ids[j / 3])
            {
                m_meshParts.push_back(spPart);
                newPart = true;
            }
            
            if (newPart)
            {
                newPart = false;
                spPart = std::make_shared<MeshPart>();
                spPart->MaterialID = m.material_ids[j / 3];
                spPart->PartName = shape.name;
            }

            // !! Fix me for quads, etc.
            assert(m.num_face_vertices[j / 3] == 3);
            for (int v = 0; v < m.num_face_vertices[j / 3]; v++)
            {
                tinyobj::index_t index = m.indices[j + v];
                glm::vec3 pos = glm::vec3(m_attrib.vertices[index.vertex_index * 3 + 0],
                    m_attrib.vertices[index.vertex_index * 3 + 1],
                    m_attrib.vertices[index.vertex_index * 3 + 2]);

                glm::vec3 normal = glm::vec3(m_attrib.normals[index.normal_index * 3 + 0],
                    m_attrib.normals[index.normal_index * 3 + 1],
                    m_attrib.normals[index.normal_index * 3 + 2]);

                glm::vec2 tex = glm::vec2(m_attrib.texcoords[index.texcoord_index * 2 + 0],
                    m_attrib.normals[index.texcoord_index * 2 + 1]);

                spPart->Positions.push_back(pos);
                spPart->Normals.push_back(normal);
                spPart->UVs.push_back(tex);
            }
        }

        if (!newPart)
        {
            m_meshParts.push_back(spPart);
        }
    }
    return true;
}
