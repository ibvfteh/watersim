#pragma once

#include "renderer/vulkan/swap_chain/vulkan_command_pool.h"
#include "renderer/vulkan/swap_chain/vulkan_render_pass.h"
#include "renderer/vulkan/swap_chain/vulkan_framebuffers.h"
#include "renderer/vulkan/material/vulkan_pipeline.h"
#include "renderer/vulkan/material/vulkan_descriptor_sets.h"
#include "renderer/vulkan/buffers/vulkan_vertex_buffer.h"
#include "renderer/vulkan/buffers/vulkan_index_buffer.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace estun
{
    class VulkanCommandBuffers
    {
    private:
        std::vector<VkCommandBuffer> commandBuffers;
		VkRenderPassBeginInfo        renderPassInfo;
	    VulkanCommandPool*           currCommandPool;
    public:
        explicit VulkanCommandBuffers(VulkanCommandPool* commandPool, int16_t cb_size);
        ~VulkanCommandBuffers();

        void InitCommandBuffers(
            VulkanRenderPass* renderPass, 
            VkExtent2D* swapChainExtent, 
            VulkanFramebuffers* vkSwapChainFramebuffers);
        void CloseCommandBuffers();
        void ResetCommandBuffers();
        void FreeCommandBuffers();

        void BindShader(std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline, VulkanDescriptorSets* descriptorSets);
        void LoadDraw(VulkanVertexBuffer* vertexBuffer, VulkanIndexBuffer* indexBuffer);

        std::vector<VkCommandBuffer>* GetCommandBuffersVector();
    };
}
