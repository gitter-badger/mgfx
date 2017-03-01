#include "common.h"
#include "fileutils.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "mesh.h"
#include "geometry/indexer.h"

bool Mesh::Load(const fs::path& modelPath)
{
    if (!fs::exists(modelPath))
    {
        return false;
    }

    m_rootPath = GetDir(modelPath);

    std::string err;
    bool ret = tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &err, modelPath.string().c_str(), (GetDir(modelPath).string() + "/").c_str(), true);
    if (!err.empty())
    {
        LOG(ERROR) << err;
    }
    if (!ret)
    {
        return false;
    }

    std::shared_ptr<MeshPart> spPart;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    auto finishPart = [&](std::shared_ptr<MeshPart>& spPart)
    {
        if (positions.empty())
        {
            return;
        }

        // Re-index the mesh part
        indexVBO(positions, texCoords, normals, spPart->Indices, spPart->Positions, spPart->UVs, spPart->Normals);

        m_meshParts.push_back(spPart);
    };

    for (auto& shape : m_shapes)
    {
        tinyobj::mesh_t& m = shape.mesh;

        for (size_t j = 0; j < m.indices.size(); j += 3)
        {
            if (!spPart ||
                spPart->MaterialID != m.material_ids[j / 3])
            {
                if (spPart)
                {
                    finishPart(spPart);
                }

                spPart = std::make_shared<MeshPart>();
                spPart->MaterialID = m.material_ids[j / 3];
                spPart->PartName = shape.name;

                positions.clear();
                normals.clear();
                texCoords.clear();
            }

            // Note: we ask for triangulation of the data
            assert(m.num_face_vertices[j / 3] == 3);
            for (int v = 0; v < m.num_face_vertices[j / 3]; v++)
            {
                tinyobj::index_t index = m.indices[j + v];
                glm::vec3 pos = glm::vec3(m_attrib.vertices[index.vertex_index * 3 + 0],
                    m_attrib.vertices[index.vertex_index * 3 + 1],
                    m_attrib.vertices[index.vertex_index * 3 + 2]);

                glm::vec3 normal;
                if (!m_attrib.normals.empty())
                {
                    normal = glm::vec3(m_attrib.normals[index.normal_index * 3 + 0],
                        m_attrib.normals[index.normal_index * 3 + 1],
                        m_attrib.normals[index.normal_index * 3 + 2]);
                }

                glm::vec2 tex;
                if (!m_attrib.texcoords.empty())
                {
                    tex = glm::vec2(m_attrib.texcoords[index.texcoord_index * 2 + 0],
                        m_attrib.texcoords[index.texcoord_index * 2 + 1]);
                }

                positions.push_back(pos);
                normals.push_back(normal);
                texCoords.push_back(tex);
            }
        }

        if (spPart)
        {
            finishPart(spPart);
            spPart = nullptr;
        }
    }
    return true;
}
