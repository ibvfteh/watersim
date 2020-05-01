#pragma once

#include "renderer/common.h"

namespace estun
{

    class RenderPass;
    class Descriptor;
    class ShaderModule;

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(const GraphicsPipeline &) = delete;
        GraphicsPipeline(GraphicsPipeline &&) = delete;

        GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;
        GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

        GraphicsPipeline(
            const std::string vertexShaderName,
            const std::string fragmentShaderName,
            std::unique_ptr<RenderPass> &renderPass,
            std::shared_ptr<Descriptor> descriptor,
            VkSampleCountFlagBits msaa,
            bool isWireFrame);
        ~GraphicsPipeline();

        void Create(std::unique_ptr<RenderPass> &renderPass);
        void Destroy();
        void Recreate(std::unique_ptr<RenderPass> &renderPass);

        void Bind(VkCommandBuffer &commandBuffer);

        VkDescriptorSet GetDescriptorSet(uint32_t index) const;
        bool IsWireFrame() const { return isWireFrame_; }

    private:
        const bool isWireFrame_;
        VkSampleCountFlagBits msaa_;

        VkPipeline pipeline_;

        std::shared_ptr<Descriptor> descriptor_;

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages_;
        std::unique_ptr<ShaderModule> vertShaderModule_;
        std::unique_ptr<ShaderModule> fragShaderModule_;
    };

} // namespace estun