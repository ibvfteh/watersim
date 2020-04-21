#include "Renderer/Vulkan/Context/Surface.h"
#include "Renderer/Vulkan/Context/Instance.h"
#include "Renderer/Vulkan/Context/Utils.h"

Estun::Renderer::Vulkan::Surface::Surface(Estun::Renderer::Vulkan::Instance *instance, GLFWwindow *window)
{
    VK_CHECK_RESULT(glfwCreateWindowSurface(*instance->GetVulkanInstance(), window, nullptr, &surface), "Failed to create window surface!");
}

void Estun::Renderer::Vulkan::Surface::Delete(Instance *instance)
{
    vkDestroySurfaceKHR(*instance->GetVulkanInstance(), surface, nullptr);
}

VkSurfaceKHR Estun::Renderer::Vulkan::Surface::GetSurface() const
{
    return surface;
}
