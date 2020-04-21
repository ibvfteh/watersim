#include "Renderer/Vulkan/Context/CommandPool.h"
#include "Renderer/Vulkan/Context/Device.h"

Estun::Renderer::Vulkan::CommandPool::CommandPool(CommandPoolType type)
{
    QueueFamilyIndices queueFamilyIndices = DeviceLocator::GetDevice().GetQueueFamilyIndices();
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    switch (type)
    {
    case Graphics:
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        break;
    case Compute:
        poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();
        break;
    case Transfer:
        poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();
        break;
    }
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CHECK_RESULT(vkCreateCommandPool(DeviceLocator::GetLogicalDevice(), &poolInfo, nullptr, &commandPool), "Failed to create command pool");
}

Estun::Renderer::Vulkan::CommandPool::~CommandPool()
{
    vkDestroyCommandPool(DeviceLocator::GetLogicalDevice(), commandPool, nullptr);
}

void Estun::Renderer::Vulkan::CommandPool::Reset()
{
    vkResetCommandPool(DeviceLocator::GetLogicalDevice(), commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}

VkCommandPool Estun::Renderer::Vulkan::CommandPool::GetCommandPool() const
{
    return commandPool;
}

Estun::Renderer::Vulkan::CommandPool *Estun::Renderer::Vulkan::CommandPoolLocator::currGraphicsCommandPool;

Estun::Renderer::Vulkan::CommandPool *Estun::Renderer::Vulkan::CommandPoolLocator::currComputeCommandPool;

Estun::Renderer::Vulkan::CommandPool *Estun::Renderer::Vulkan::CommandPoolLocator::currTransferCommandPool;
