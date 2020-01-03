#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace estun
{
    class VulkanInstance;

    class VulkanValidationLayers
    {
    private:
        VkDebugUtilsMessengerEXT debugMessenger;

#ifdef NDEBUG
		static const bool enableValidationLayers = false;
#else
		static const bool enableValidationLayers = true;
#endif

        std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT              messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
            void*                                       pUserData);

        VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger) ;
        
        void DestroyDebugUtilsMessengerEXT(
            VkInstance instance,
            VkDebugUtilsMessengerEXT debugMessenger, 
            const VkAllocationCallbacks* pAllocator);
    public:
        VulkanValidationLayers();
        ~VulkanValidationLayers() = default;

        void Delete(VulkanInstance* instance);

        void SetupDebugMessenger(VulkanInstance* instance);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        bool CheckValidationLayers();
        static bool IsEnabled();
        std::vector<const char*>* GetLayers();
    };
}
