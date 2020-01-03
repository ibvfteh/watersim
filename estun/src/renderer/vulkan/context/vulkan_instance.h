#pragma once

#include "renderer/vulkan/context/vulkan_valid_layers.h"
#include "core/core.h"

namespace estun 
{
    class VulkanInstance 
    {
    private:
        
        VkInstance vk_instance;

        VulkanValidationLayers* valLayers = nullptr;
    public:
        VulkanInstance(
                  const char* app_name,
                  const int app_version,
                  const char* engine_name = "estun",
                  const int engine_version = 10000
                  );
        ~VulkanInstance();
        VkInstance* GetVulkanInstance();
        VulkanValidationLayers* GetValidationLayers();

    private:

        void CreateInstance(const char* app_name,
                       const int app_version,
                       const char* engine_name,
                       const int engine_version);
    };
}
