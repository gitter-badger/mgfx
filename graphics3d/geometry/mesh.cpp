#include "common.h"
#include "fileutils.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "mesh.h"
#include "geometry/tangentspace.h"
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

    // Note:
    // In this demo I am building lists of triangles for each part of the mesh that has a different material.
    // A further optimization is to index the triangles.
    // This saves bandwidth when sending geometry, and memory space.
    // It makes things a bit more complicated though, because you have to build index lists based on material/vertex groups,
    // as well as sending them to the card.
    // And on modern devices, your geometry bandwidth may not be the most costly thing.
    // Indexing geometry could be considered premature optimization, and is easy low-hanging fruit if you need to do it!
    std::shared_ptr<MeshPart> spPart;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> binormals;
    std::vector<glm::vec2> texCoords;

    auto finishPart = [&](std::shared_ptr<MeshPart>& spPart)
    {
        // Compute the tangent and binormal vectors for the mesh (used in bump mapping)
        computeTangentBasis(positions, texCoords, normals, tangents, binormals);

#ifdef _DEBUG
        for (uint32_t i = 0; i < positions.size(); i++)
        {
            spPart->Indices.push_back(i);
        }
        spPart->Positions = positions;
        spPart->Normals = normals;
        spPart->Tangents = tangents;
        spPart->Binormals = binormals;
        spPart->UVs = texCoords;
#else
        // Re-index the mesh part
        indexVBO_TBN(positions, texCoords, normals, tangents, binormals,
            spPart->Indices, spPart->Positions, spPart->UVs, spPart->Normals, spPart->Tangents, spPart->Binormals);
#endif

        m_meshParts.push_back(spPart);
    };

    for (auto& shape : m_shapes)
    {
        tinyobj::mesh_t& m = shape.mesh;

        bool newPart = true;
        for (size_t j = 0; j < m.indices.size(); j += 3)
        {
            if (spPart &&
                spPart->MaterialID != m.material_ids[j / 3])
            {
                finishPart(spPart);
                newPart = true;
            }
            
            if (newPart)
            {
                newPart = false;
                spPart = std::make_shared<MeshPart>();
                spPart->MaterialID = m.material_ids[j / 3];
                spPart->PartName = shape.name;

                positions.clear();
                normals.clear();
                tangents.clear();
                binormals.clear();
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

        if (!newPart)
        {
            finishPart(spPart);
        }
    }
    return true;
}
