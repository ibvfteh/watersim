#include "renderer/vulkan/context/vulkan_device.h"

#include <set>
#include <iostream>

namespace estun
{
    VulkanDevice* VulkanDeviceLocator::currDevice = nullptr;

    VulkanDevice::~VulkanDevice()
    {
        vkDestroyDevice(logicalDevice, nullptr);
    }

    VulkanDevice::VulkanDevice(VulkanInstance* instance, VulkanSurface* surface)
    {
        PickPhysicalDevice(instance, surface);
        CreateLogicalDevice(instance, surface);
    }

    QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) 
    {
        QueueFamilyIndices indices;
        
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) 
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        
        return indices;
    }


    bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device, VulkanSurface* surface, int curr_deviceId)
    {
        QueueFamilyIndices indices = FindQueueFamilies(device, *surface->GetSurface());
    
        bool extensionsSupported = CheckDeviceExtensionSupport(device);
        
        bool swapChainAdequate = false;
        if (extensionsSupported) 
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        if (deviceId == curr_deviceId)
        {
            return indices.isComplete() && extensionsSupported && swapChainAdequate;
        }

        return false;
    }
    
    VkSampleCountFlagBits VulkanDevice::GetMaxUsableSampleCount() 
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT)  { return VK_SAMPLE_COUNT_8_BIT;  }
        if (counts & VK_SAMPLE_COUNT_4_BIT)  { return VK_SAMPLE_COUNT_4_BIT;  }
        if (counts & VK_SAMPLE_COUNT_2_BIT)  { return VK_SAMPLE_COUNT_2_BIT;  }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    void VulkanDevice::PickPhysicalDevice(VulkanInstance* instance, VulkanSurface* surface)
    {
        physicalDevice = VK_NULL_HANDLE;
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance->GetVulkanInstance(), &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            ES_CORE_ASSERT("Failed to find GPUs with Vulkan support");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(*instance->GetVulkanInstance(), &deviceCount, devices.data());
        int i = 0;
        for (const auto& device : devices) 
        {
            if (IsDeviceSuitable(device, surface, i))
            {
                physicalDevice = device;
                break;
            }
            i++;
        }

        if (physicalDevice == VK_NULL_HANDLE)
        {
            ES_CORE_WARN("Failed to find a specific GPU");
            for (const auto& device : devices)
            {
                physicalDevice = device;
                break;
            }
        }

        msaaSamples = GetMaxUsableSampleCount();

        if (physicalDevice == VK_NULL_HANDLE) 
        {
            ES_CORE_CRITICAL("Failed to find any suitable GPU");        if (physicalDevice == 0) 
        {
            ES_CORE_CRITICAL("Failed to find any suitable GPU 2");
        }
        }
    }

    void VulkanDevice::CreateLogicalDevice(VulkanInstance* instance, VulkanSurface* surface)
    {
        currIndices = FindQueueFamilies(physicalDevice, *surface->GetSurface());

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {currIndices.graphicsFamily.value(), currIndices.presentFamily.value()};
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) 
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount       = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos       = queueCreateInfos.data();
        createInfo.pEnabledFeatures        = &deviceFeatures;

        createInfo.enabledExtensionCount   = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (VulkanValidationLayers::IsEnabled()) 
        {
            createInfo.enabledLayerCount   = static_cast<uint32_t>(instance->GetValidationLayers()->GetLayers()->size());
            createInfo.ppEnabledLayerNames = instance->GetValidationLayers()->GetLayers()->data();
        } 
        else 
        {
            createInfo.enabledLayerCount   = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) 
        {
            ES_CORE_ASSERT("Failed to create logical device");
        }

        vkGetDeviceQueue(logicalDevice, currIndices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, currIndices.presentFamily.value(), 0, &presentQueue);    
    }

    
    QueueFamilyIndices VulkanDevice::GetQueueFamilyIndices() const
    {
	    return currIndices;
    }

    VkSampleCountFlagBits VulkanDevice::GetMsaaSamples() const
    {
        return msaaSamples;
    }

    VkPhysicalDevice* VulkanDevice::GetPhysicalDevice()
    {
        return &physicalDevice;
    }

    VkDevice* VulkanDevice::GetLogicalDevice()
    {
        return &logicalDevice;
    }

    VkQueue* VulkanDevice::GetGraphicsQueue() 
    {
        return &graphicsQueue;
    }
    
    VkQueue* VulkanDevice::GetPresentQueue() 
    {
        return &presentQueue;
    }

    SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice device, VulkanSurface* surface)
    {
        SwapChainSupportDetails details;

        VkSurfaceKHR* rawSurface = surface->GetSurface();

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, *rawSurface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, *rawSurface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, *rawSurface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, *rawSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, *rawSurface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }
}
