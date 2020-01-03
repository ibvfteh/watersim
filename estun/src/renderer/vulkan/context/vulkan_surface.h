#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "core/core.h"
#include "renderer/vulkan/context/vulkan_instance.h"
#include <GLFW/glfw3.h>

namespace estun
{
    class VulkanSurface
    {
    private:
        VkSurfaceKHR surface;

    public:
        VulkanSurface(VulkanInstance* instance, GLFWwindow* window);
        ~VulkanSurface() = default;
        void Delete(VulkanInstance* instance);

        VkSurfaceKHR* GetSurface();
    };
}
