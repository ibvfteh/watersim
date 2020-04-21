#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Core/Core.h"
#include <GLFW/glfw3.h>

namespace Estun
{

namespace Renderer
{
    
namespace Vulkan
{

class Instance;

class Surface
{
private:
    VkSurfaceKHR surface;

public:
    Surface(const Surface &) = delete;
    Surface(Surface &&) = delete;

    Surface &operator=(const Surface &) = delete;
    Surface &operator=(Surface &&) = delete;

    Surface(Instance *instance, GLFWwindow *window);
    ~Surface() = default;
    
    void Delete(Instance *instance);

    VkSurfaceKHR GetSurface() const;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
