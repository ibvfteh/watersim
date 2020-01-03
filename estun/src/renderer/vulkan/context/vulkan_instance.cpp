#include "renderer/vulkan/context/vulkan_instance.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>

namespace estun
{
    VulkanInstance::VulkanInstance(const char* app_name,
                                    const int app_version,
                                    const char* engine_name,
                                    const int engine_version)
    {
        if (VulkanValidationLayers::IsEnabled())
        {
            valLayers = new VulkanValidationLayers();
        }
	    CreateInstance(app_name, app_version, engine_name, engine_version);
	    if (valLayers)
        {
            valLayers->SetupDebugMessenger(this);
        }
    }

    VulkanInstance::~VulkanInstance()
    {
        if (valLayers)
        {
            valLayers->Delete(this);
            delete valLayers;
        }

	    vkDestroyInstance(vk_instance, nullptr);
    }

    std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (VulkanValidationLayers::IsEnabled()) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void VulkanInstance::CreateInstance(const char* app_name,
                                       const int app_version,
                                       const char* engine_name,
                                       const int engine_version)

    {
        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName   = app_name;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(app_version / 10000, app_version % 10000 / 100, app_version % 100);
        applicationInfo.pEngineName        = engine_name;
        applicationInfo.engineVersion      = VK_MAKE_VERSION(engine_version / 10000, engine_version % 10000 / 100, engine_version % 100);
        applicationInfo.apiVersion         = VK_API_VERSION_1_0;
        
        auto extensions = GetRequiredExtensions();

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        std::vector<const char*> enabledLayers;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (valLayers)
        {
            enabledLayers = *valLayers->GetLayers();

            createInfo.enabledLayerCount   = static_cast<uint32_t>(enabledLayers.size());
            createInfo.ppEnabledLayerNames = enabledLayers.data();

            valLayers->populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext               = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount   = 0;

            createInfo.pNext               = nullptr;
        }

        if(vkCreateInstance(&createInfo, NULL, &vk_instance) != VK_SUCCESS)
        {
            ES_CORE_ASSERT("Failed to create vulkan instance");
        }
    }
   		
    VkInstance* VulkanInstance::GetVulkanInstance()
    {
	    return &vk_instance;
    }

    VulkanValidationLayers* VulkanInstance::GetValidationLayers()
    {
        return valLayers;
    }
}
