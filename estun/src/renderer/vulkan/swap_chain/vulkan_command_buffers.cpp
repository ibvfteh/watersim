#include "renderer/vulkan/swap_chain/vulkan_command_buffers.h"
#include "renderer/vulkan/context/vulkan_device.h"
#include "core/core.h"

namespace estun
{
    VulkanCommandBuffers::VulkanCommandBuffers(VulkanCommandPool* commandPool, int16_t cb_size)
    {
        currCommandPool = commandPool;

        commandBuffers.resize(cb_size);

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = *commandPool->GetCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(*VulkanDeviceLocator::GetLogicalDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) 
        {
            ES_CORE_ASSERT("Failed to allocate command buffers");
        }
    }

    VulkanCommandBuffers::~VulkanCommandBuffers()
    {
        FreeCommandBuffers();
    }

    void VulkanCommandBuffers::InitCommandBuffers(VulkanRenderPass* renderPass, VkExtent2D* swapChainExtent, VulkanFramebuffers* vkSwapChainFramebuffers)
    {
        for (int i = 0; i < commandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) 
            {
                ES_CORE_ASSERT("Failed to begin recording command buffer");
            }
            
            std::vector<VkFramebuffer>* swapChainFramebuffers = vkSwapChainFramebuffers->GetSwapChainFramebuffersVector();
            VkRenderPassBeginInfo renderPassInfo = {};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = *renderPass->GetRenderPass();
            renderPassInfo.framebuffer = (*swapChainFramebuffers)[i];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = *swapChainExtent;

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        }
    }

    void VulkanCommandBuffers::BindShader(VulkanGraphicsPipeline* graphicsPipeline, VulkanDescriptorSets* descriptorSets)
    {
        for (int i = 0; i < commandBuffers.size(); i++) 
        {
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, *graphicsPipeline->GetGraphicsPipeline());

            vkCmdBindDescriptorSets(
                    commandBuffers[i],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    *graphicsPipeline->GetPipelineLayout(),
                    0, 1,
                    &(*descriptorSets->GetDescriptorSets())[i],
                    0, nullptr);
        }
    }

    void VulkanCommandBuffers::LoadDraw(VulkanVertexBuffer* vertexBuffer, VulkanIndexBuffer* indexBuffer)
    {
        for (auto& commandBuffer : commandBuffers) 
        {
            VkBuffer vertexBuffers[] = {*vertexBuffer->GetBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(commandBuffer, *indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indexBuffer->GetSize()), 1, 0, 0, 0);
        }
    }

    void VulkanCommandBuffers::CloseCommandBuffers()
    {
        for (auto& commandBuffer : commandBuffers)
        {
            vkCmdEndRenderPass(commandBuffer);

            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            {
                ES_CORE_ASSERT("Failed to record command buffer");
            }
        }
    }
/*
    void VulkanCommandBuffers::ResetCommandBuffers()
    {
        for (auto& commandBuffer : commandBuffers)
        {
            vkResetCommandBuffer(commandBuffer, nullptr);
        }
    }
*/
    void VulkanCommandBuffers::FreeCommandBuffers()
    {
        if (!commandBuffers.empty())
        {
            vkFreeCommandBuffers(*VulkanDeviceLocator::GetLogicalDevice(), *currCommandPool->GetCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
            commandBuffers.clear();
        }
    }

    std::vector<VkCommandBuffer>* VulkanCommandBuffers::GetCommandBuffersVector()
    {
        return &commandBuffers;
    }
}
