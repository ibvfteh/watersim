#pragma once
#include <vulkan/vulkan.hpp>
#include <renderer/vulkan/context/vulkan_device.h>

namespace estun
{
	class MemoryManager
	{
	public:
		static uint32_t FindMemoryType(VulkanDevice* device, const uint32_t& typeFilter, const VkMemoryPropertyFlags& properties);
	};
}