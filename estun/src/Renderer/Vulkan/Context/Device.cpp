#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Context/Instance.h"
#include "Renderer/Vulkan/Context/Surface.h"
#include "Renderer/Vulkan/Context/ValidationLayers.h"

#include <set>
#include <iostream>

Estun::Renderer::Vulkan::Device *Estun::Renderer::Vulkan::DeviceLocator::currDevice = nullptr;

Estun::Renderer::Vulkan::Device::~Device()
{
    vkDestroyDevice(logicalDevice, nullptr);
}

Estun::Renderer::Vulkan::Device::Device(Estun::Renderer::Vulkan::Instance *instance, Estun::Renderer::Vulkan::Surface *surface)
{
    PickPhysicalDevice(instance, surface);
    CreateLogicalDevice(instance, surface);
}

uint32_t Estun::Renderer::Vulkan::Device::GetQueueFamilyIndex(std::vector<VkQueueFamilyProperties> queueFamilyProperties, VkQueueFlagBits queueFlags)
{
    if (queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
            {
                return i;
                break;
            }
        }
    }

    if (queueFlags & VK_QUEUE_TRANSFER_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
        {
            if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
            {
                return i;
                break;
            }
        }
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
    {
        if (queueFamilyProperties[i].queueFlags & queueFlags)
        {
            return i;
            break;
        }
    }

    ES_CORE_ASSERT("Could not find a matching queue family index");
}

Estun::Renderer::Vulkan::QueueFamilyIndices Estun::Renderer::Vulkan::Device::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    indices.graphicsFamily = GetQueueFamilyIndex(queueFamilies, VK_QUEUE_GRAPHICS_BIT);
    indices.computeFamily = GetQueueFamilyIndex(queueFamilies, VK_QUEUE_COMPUTE_BIT);
    indices.transferFamily = GetQueueFamilyIndex(queueFamilies, VK_QUEUE_TRANSFER_BIT);

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }
    }

    return indices;
}

bool Estun::Renderer::Vulkan::Device::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }
    
    for (const auto &extension : requiredExtensions)
    {
        ES_CORE_WARN(std::string("Could not find extention: ") + extension);
    }

    return requiredExtensions.empty();
}

bool Estun::Renderer::Vulkan::Device::IsDeviceSuitable(VkPhysicalDevice device, Estun::Renderer::Vulkan::Surface *surface)
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface->GetSurface());

    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

VkSampleCountFlagBits Estun::Renderer::Vulkan::Device::GetMaxUsableSampleCount()
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT)
    {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_32_BIT)
    {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_16_BIT)
    {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_8_BIT)
    {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_4_BIT)
    {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (counts & VK_SAMPLE_COUNT_2_BIT)
    {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

void Estun::Renderer::Vulkan::Device::PickPhysicalDevice(Estun::Renderer::Vulkan::Instance *instance, Estun::Renderer::Vulkan::Surface *surface)
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

    for (const auto &device : devices)
    {
        if (IsDeviceSuitable(device, surface))
        {
            physicalDevice = device;
            msaaSamples = GetMaxUsableSampleCount();
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
    {
        ES_CORE_ASSERT("Failed to find a suitable GPU");
    }
}

void Estun::Renderer::Vulkan::Device::CreateLogicalDevice(Estun::Renderer::Vulkan::Instance *instance, Estun::Renderer::Vulkan::Surface *surface)
{
    currIndices = FindQueueFamilies(physicalDevice, surface->GetSurface());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {currIndices.graphicsFamily.value(),
                                              currIndices.presentFamily.value(),
                                              currIndices.computeFamily.value(),
                                              currIndices.transferFamily.value()};
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (ValidationLayers::IsEnabled())
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(instance->GetValidationLayers()->GetLayers()->size());
        createInfo.ppEnabledLayerNames = instance->GetValidationLayers()->GetLayers()->data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice), "Failed to create logical device");

    vkGetDeviceQueue(logicalDevice, currIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, currIndices.presentFamily.value(), 0, &presentQueue);
    vkGetDeviceQueue(logicalDevice, currIndices.computeFamily.value(), 0, &computeQueue);
    vkGetDeviceQueue(logicalDevice, currIndices.transferFamily.value(), 0, &transferQueue);
}

Estun::Renderer::Vulkan::QueueFamilyIndices Estun::Renderer::Vulkan::Device::GetQueueFamilyIndices() const
{
    return currIndices;
}

VkSampleCountFlagBits Estun::Renderer::Vulkan::Device::GetMsaaSamples() const
{
    return msaaSamples;
}

VkPhysicalDevice &Estun::Renderer::Vulkan::Device::GetPhysicalDevice()
{
    return physicalDevice;
}

VkDevice &Estun::Renderer::Vulkan::Device::GetLogicalDevice()
{
    return logicalDevice;
}

VkQueue Estun::Renderer::Vulkan::Device::GetGraphicsQueue()
{
    return graphicsQueue;
}

VkQueue Estun::Renderer::Vulkan::Device::GetPresentQueue()
{
    return presentQueue;
}

VkQueue Estun::Renderer::Vulkan::Device::GetComputeQueue()
{
    return computeQueue;
}

VkQueue Estun::Renderer::Vulkan::Device::GetTransferQueue()
{
    return transferQueue;
}

void Estun::Renderer::Vulkan::Device::WaitIdle() const
{
    VK_CHECK_RESULT(vkDeviceWaitIdle(logicalDevice), "Wait for device idle");
}

Estun::Renderer::Vulkan::SwapChainSupportDetails
Estun::Renderer::Vulkan::Device::QuerySwapChainSupport(VkPhysicalDevice device, Estun::Renderer::Vulkan::Surface *surface)
{
    SwapChainSupportDetails details;

    VkSurfaceKHR rawSurface = surface->GetSurface();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, rawSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, rawSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, rawSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, rawSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, rawSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

Estun::Renderer::Vulkan::Device &Estun::Renderer::Vulkan::DeviceLocator::GetDevice()
{
    if (currDevice == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan device");
    }
    return *currDevice;
};

VkDevice &Estun::Renderer::Vulkan::DeviceLocator::GetLogicalDevice()
{
    if (currDevice == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan device");
    }
    return currDevice->GetLogicalDevice();
};

VkPhysicalDevice &Estun::Renderer::Vulkan::DeviceLocator::GetPhysicalDevice()
{
    if (currDevice == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan device");
    }
    return currDevice->GetPhysicalDevice();
};
