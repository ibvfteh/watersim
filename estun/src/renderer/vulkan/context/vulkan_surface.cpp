#include "renderer/vulkan/context/vulkan_surface.h"

namespace estun 
{
    VulkanSurface::VulkanSurface(VulkanInstance* instance, GLFWwindow* window)
    {
        if (glfwCreateWindowSurface(*instance->GetVulkanInstance(), window, nullptr, &surface) != VK_SUCCESS)
        {
            ES_CORE_ASSERT("Failed to create window surface!");
        }
    }

    void VulkanSurface::Delete(VulkanInstance* instance)
    {
        vkDestroySurfaceKHR(*instance->GetVulkanInstance(), surface, nullptr);
    }

    VkSurfaceKHR* VulkanSurface::GetSurface()
    {
        return &surface;
    }
}
