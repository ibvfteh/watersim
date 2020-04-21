#pragma once

#include <vulkan/vulkan.h>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Vertex
{
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    int32_t materialIndex;

public:
    bool operator==(const Vertex &other) const
    {
        return position == other.position &&
               normal == other.normal &&
               texCoord == other.texCoord &&
               materialIndex == other.materialIndex;
    }

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32_SINT;
        attributeDescriptions[3].offset = offsetof(Vertex, materialIndex);

        return attributeDescriptions;
    }
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun

namespace std
{

template <>
struct hash<Estun::Renderer::Vulkan::Vertex>
{
    size_t operator()(Estun::Renderer::Vulkan::Vertex const &vertex) const noexcept
    {
        return Combine(hash<glm::vec3>()(vertex.position),
                       Combine(hash<glm::vec3>()(vertex.normal),
                               Combine(hash<glm::vec2>()(vertex.texCoord),
                                       hash<int>()(vertex.materialIndex))));
    }

private:
    static size_t Combine(size_t hash0, size_t hash1)
    {
        return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
    }
};

} // namespace std
