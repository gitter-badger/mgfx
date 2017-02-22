#pragma once

#include <tinyobjloader/tiny_obj_loader.h>

struct MeshPart
{
    std::string PartName;

    int32_t MaterialID;

    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> UVs;
};

class Mesh
{
public:
    bool Load(const char* pszFile);

    const tinyobj::attrib_t& GetAttrib() { return m_attrib; }
    const std::vector<tinyobj::shape_t>& GetShapes() const { return m_shapes; }
    const std::vector<tinyobj::material_t>& GetMaterials() const { return m_materials; }

    const std::vector<std::shared_ptr<MeshPart>>& GetMeshParts() const { return m_meshParts; }

private:
    std::vector<std::shared_ptr<MeshPart>> m_meshParts;

    tinyobj::attrib_t m_attrib;
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;
};
