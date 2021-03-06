#pragma once

#include "renderer/common.h"
#include "renderer/context/command_buffers.h"
#include "renderer/context/command_pool.h"
#include "renderer/context/device.h"
#include "renderer/context/utils.h"

namespace estun
{

class SingleTimeCommands
{
public:
    static void SubmitGraphics(CommandPool &commandPool, const std::function<void(VkCommandBuffer)> &action)
    {
        CommandBuffers commandBuffers(commandPool, 1);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

        action(commandBuffers[0]);

        vkEndCommandBuffer(commandBuffers[0]);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];

        const auto queue = DeviceLocator::GetDevice().GetGraphicsQueue();

        vkQueueSubmit(queue, 1, &submitInfo, nullptr);
        vkQueueWaitIdle(queue);
    }

    static void SubmitCompute(CommandPool &commandPool, const std::function<void(VkCommandBuffer)> &action)
    {
        CommandBuffers commandBuffers(commandPool, 1);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

        action(commandBuffers[0]);

        vkEndCommandBuffer(commandBuffers[0]);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];

        const auto queue = DeviceLocator::GetDevice().GetComputeQueue();

        vkQueueSubmit(queue, 1, &submitInfo, nullptr);
        vkQueueWaitIdle(queue);
    }

    static void SubmitTransfer(CommandPool &commandPool, const std::function<void(VkCommandBuffer)> &action)
    {
        CommandBuffers commandBuffers(commandPool, 1);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffers[0], &beginInfo);

        action(commandBuffers[0]);

        vkEndCommandBuffer(commandBuffers[0]);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[0];

        const auto queue = DeviceLocator::GetDevice().GetTransferQueue();

        vkQueueSubmit(queue, 1, &submitInfo, nullptr);
        vkQueueWaitIdle(queue);
    }
};

} // namespace estun