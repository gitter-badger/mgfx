#pragma once

#include <tinyobjloader/tiny_obj_loader.h>

class Mesh
{
public:
    bool Load(const char* pszFile);

    const tinyobj::attrib_t& GetAttrib() { return m_attrib; }
    const std::vector<tinyobj::shape_t>& GetShapes() const { return m_shapes; }
    const std::vector<tinyobj::material_t> GetMaterials() const { return m_materials; }

private:
    tinyobj::attrib_t m_attrib;
    std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;
};