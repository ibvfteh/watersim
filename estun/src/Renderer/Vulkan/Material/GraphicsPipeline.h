#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class RenderPass;
class Descriptor;

class GraphicsPipeline final
{
public:
    GraphicsPipeline(const GraphicsPipeline &) = delete;
    GraphicsPipeline(GraphicsPipeline &&) = delete;

    GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
    GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

    GraphicsPipeline(
        const std::string vertexShaderName,
        const std::string fragmentShaderName,
        RenderPass &renderPass,
        const Descriptor &descriptor,
        bool isWireFrame);
    ~GraphicsPipeline();

    void Bind();

    VkDescriptorSet GetDescriptorSet(uint32_t index) const;
    bool IsWireFrame() const { return wireFrame; }

private:
    const bool wireFrame;

    VkPipeline pipeline;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun