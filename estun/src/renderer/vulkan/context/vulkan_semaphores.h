#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace estun
{
    class VulkanSemaphoresManager
    {
    private:
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        uint32_t imageCount = -1;
    public:
        VulkanSemaphoresManager(const uint32_t imageCount_t);
        ~VulkanSemaphoresManager();

        const std::vector<VkSemaphore>* GetImageAvailableSemaphores() const;
        const std::vector<VkSemaphore>* GetRenderFinishedSemaphores() const;
        const std::vector<VkFence>* GetInFlightFences() const;
    };
}
