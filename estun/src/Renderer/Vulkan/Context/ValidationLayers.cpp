#include "Renderer/Vulkan/Context/ValidationLayers.h"
#include "Renderer/Vulkan/Context/Instance.h"
#include "Renderer/Vulkan/Context/Utils.h"
#include "Core/Core.h"
#include <iostream>

Estun::Renderer::Vulkan::ValidationLayers::ValidationLayers()
{
    if (enableValidationLayers && !CheckValidationLayers())
    {
        ES_CORE_ASSERT("Validation layer not supported");
    }
}

void Estun::Renderer::Vulkan::ValidationLayers::Delete(Instance *instance)
{
    DestroyDebugUtilsMessengerEXT(*instance->GetVulkanInstance(), debugMessenger, nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL Estun::Renderer::Vulkan::ValidationLayers::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        ES_CORE_ERROR("Validation layer:");
        ES_CORE_ERROR(pCallbackData->pMessage);
    }
    else
    {
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            ES_CORE_WARN("Validation layer:");
            ES_CORE_WARN(pCallbackData->pMessage);
        }
        else
        {
            ES_CORE_INFO("Validation layer:");
            ES_CORE_INFO(pCallbackData->pMessage);
        }
    }
    return VK_FALSE;
}

bool Estun::Renderer::Vulkan::ValidationLayers::CheckValidationLayers()
{

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }
    return true;
}

void Estun::Renderer::Vulkan::ValidationLayers::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkResult Estun::Renderer::Vulkan::ValidationLayers::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        ES_CORE_ASSERT("Validation layer extension not present");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Estun::Renderer::Vulkan::ValidationLayers::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void Estun::Renderer::Vulkan::ValidationLayers::SetupDebugMessenger(Instance *instance)
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessengerCreateInfo(createInfo);
    VK_CHECK_RESULT(CreateDebugUtilsMessengerEXT(*instance->GetVulkanInstance(), &createInfo, nullptr, &debugMessenger), "Failed to set up debug messenger")
}

bool Estun::Renderer::Vulkan::ValidationLayers::IsEnabled()
{
    return enableValidationLayers;
}

std::vector<const char *> *Estun::Renderer::Vulkan::ValidationLayers::GetLayers()
{
    return &validationLayers;
}
