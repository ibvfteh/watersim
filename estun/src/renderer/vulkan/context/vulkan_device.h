#pragma once

#include "core/core.h"
#include "renderer/vulkan/context/vulkan_instance.h"
#include "renderer/vulkan/context/vulkan_surface.h"

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

namespace estun
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() 
        {
            return graphicsFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

    class VulkanDevice
    {
    private:
        VkPhysicalDevice      physicalDevice;
        VkDevice              logicalDevice;
        VkSampleCountFlagBits msaaSamples;
        
        QueueFamilyIndices currIndices;
            
        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        int deviceId = 0;   
    public:
        VulkanDevice(VulkanInstance* instance, VulkanSurface* surface);
        ~VulkanDevice();

        VkPhysicalDevice* GetPhysicalDevice();
        VkDevice*         GetLogicalDevice();
        
        static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface* surface);

        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
        QueueFamilyIndices GetQueueFamilyIndices() const;

        VkSampleCountFlagBits GetMaxUsableSampleCount();
        VkSampleCountFlagBits GetMsaaSamples() const;

        VkQueue* GetGraphicsQueue();
        VkQueue* GetPresentQueue();
    private:
        void PickPhysicalDevice(VulkanInstance* instance, VulkanSurface* surface);
        void CreateLogicalDevice(VulkanInstance* instance, VulkanSurface* surface);
 
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface* surfcae, int urr_deviceId);
    };

    class VulkanDeviceLocator
    {
    public:
        static VulkanDevice*     GetDevice()         { return currDevice; };
        static VkDevice*         GetLogicalDevice()  { return currDevice->GetLogicalDevice(); };
        static VkPhysicalDevice* GetPhysicalDevice() { return currDevice->GetPhysicalDevice(); };
 
        static void Provide(VulkanDevice* device) { currDevice = device; };
    private:
        static VulkanDevice* currDevice;
    };
}
