#include "Renderer/Vulkan/Context/Fence.h"
#include "Renderer/Vulkan/Context/Device.h"

Estun::Renderer::Vulkan::Fence::Fence(const bool signaled) 
{
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VK_CHECK_RESULT(vkCreateFence(DeviceLocator::GetLogicalDevice(), &fenceInfo, nullptr, &fence), "Failed to create fence");
}

Estun::Renderer::Vulkan::Fence::Fence(Fence&& other) noexcept :
	fence(other.fence)
{
	other.fence = nullptr;
}

Estun::Renderer::Vulkan::Fence::~Fence()
{
	if (fence != nullptr)
	{
		vkDestroyFence(DeviceLocator::GetLogicalDevice(), fence, nullptr);
		fence = nullptr;
	}
}

void Estun::Renderer::Vulkan::Fence::Reset()
{
	VK_CHECK_RESULT(vkResetFences(DeviceLocator::GetLogicalDevice(), 1, &fence), "Failed to reset fence");
}

void Estun::Renderer::Vulkan::Fence::Wait(const uint64_t timeout) const
{
	VK_CHECK_RESULT(vkWaitForFences(DeviceLocator::GetLogicalDevice(), 1, &fence, VK_TRUE, timeout), "Failed to wait for fence");
}

const VkFence &Estun::Renderer::Vulkan::Fence::GetFence() const 
{ 
    return fence; 
}