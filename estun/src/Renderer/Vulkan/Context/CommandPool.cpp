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

Estun::Renderer::Vulkan::CommandPool *Estun::Renderer::Vulkan::CommandPoolLocator::currGraphicsCommandPool = nullptr;

Estun::Renderer::Vulkan::CommandPool *Estun::Renderer::Vulkan::CommandPoolLocator::currComputeCommandPool = nullptr;

Estun::Renderer::Vulkan::CommandPool *Estun::Renderer::Vulkan::CommandPoolLocator::currTransferCommandPool = nullptr;

Estun::Renderer::Vulkan::CommandPool &Estun::Renderer::Vulkan::CommandPoolLocator::GetGraphicsPool()
{
    if (currGraphicsCommandPool == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan graphics command pool");
    }
    return *currGraphicsCommandPool;
}

Estun::Renderer::Vulkan::CommandPool &Estun::Renderer::Vulkan::CommandPoolLocator::GetComputePool()
{
    if (currComputeCommandPool == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan graphics command pool");
    }
    return *currComputeCommandPool;
}

Estun::Renderer::Vulkan::CommandPool &Estun::Renderer::Vulkan::CommandPoolLocator::GetTransferPool()
{
    if (currTransferCommandPool == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan graphics command pool");
    }
    return *currTransferCommandPool;
}

void Estun::Renderer::Vulkan::CommandPoolLocator::Provide(
    Estun::Renderer::Vulkan::CommandPool *graphicsCommandPool,
    Estun::Renderer::Vulkan::CommandPool *computeCommandPool,
    Estun::Renderer::Vulkan::CommandPool *transferCommandPool)
{
    currGraphicsCommandPool = graphicsCommandPool;
    currComputeCommandPool = computeCommandPool;
    currTransferCommandPool = transferCommandPool;
}
