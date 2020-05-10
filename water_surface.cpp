#include "water_surface.h"

namespace
{
    void AddTriangle(std::vector<uint32_t> &indices, const uint32_t offset, const uint32_t i0, const uint32_t i1, const uint32_t i2)
    {
        indices.push_back(offset + i0);
        indices.push_back(offset + i1);
        indices.push_back(offset + i2);
    }
} // namespace

estun::Model WaterSurface::CreateSurface(const float scale, const int width, const int height)
{
    std::vector<estun::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<estun::Material> materials;

    WaterSurface::Create(scale, width, height, vertices, indices, materials);

    return estun::Model(
        "surface",
        std::move(vertices),
        std::move(indices),
        std::move(materials));
}

void WaterSurface::Create(
    float scale, const int width, const int height,
    std::vector<estun::Vertex> &vertices,
    std::vector<uint32_t> &indices,
    std::vector<estun::Material> &materials)
{

    const float s = scale;

    int halfWidth = width / 2;
    int halfHeight = height / 2;

    for (int i = -halfWidth; i < halfWidth - 1; i++)
    {
        for (int j = -halfHeight; j < halfHeight - 1; j++)
        {
            const glm::vec3 v0((double)(i + 0) * s / width, 0, (double)(j + 1) * s / height);
            const glm::vec3 v3((double)(i + 0) * s / width, 0, (double)(j + 0) * s / height);
            const glm::vec3 v1((double)(i + 1) * s / width, 0, (double)(j + 1) * s / height);
            const glm::vec3 v2((double)(i + 1) * s / width, 0, (double)(j + 0) * s / height);
            
            const glm::vec2 vt0((double)(i + 0 + halfWidth) / width, (double)(j + 1 + halfHeight) / height);
            const glm::vec2 vt3((double)(i + 0 + halfWidth) / width, (double)(j + 0 + halfHeight) / height);
            const glm::vec2 vt1((double)(i + 1 + halfWidth) / width, (double)(j + 1 + halfHeight) / height);
            const glm::vec2 vt2((double)(i + 1 + halfWidth) / width, (double)(j + 0 + halfHeight) / height);

            materials.push_back(estun::Material::Lambertian(glm::vec3(0.28f, 0.82f, 0.8f))); // water color

            uint32_t i = static_cast<uint32_t>(vertices.size());
            vertices.push_back(estun::Vertex{v0, glm::vec3(0, 1, 0), vt0, 0});
            vertices.push_back(estun::Vertex{v1, glm::vec3(0, 1, 0), vt1, 0});
            vertices.push_back(estun::Vertex{v2, glm::vec3(0, 1, 0), vt2, 0});
            vertices.push_back(estun::Vertex{v3, glm::vec3(0, 1, 0), vt3, 0});

            AddTriangle(indices, i, 0, 1, 2);
            AddTriangle(indices, i, 0, 2, 3);
        }
    }
}