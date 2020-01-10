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
        VkSemaphore computeAvailableSemaphores;
        VkSemaphore computeFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;

        uint32_t imageCount = -1;
    public:
        VulkanSemaphoresManager(const uint32_t imageCount_t, const uint32_t swapChainImagesSize);
        ~VulkanSemaphoresManager();

        const std::vector<VkSemaphore>* GetImageAvailableSemaphores() const;
        const std::vector<VkSemaphore>* GetRenderFinishedSemaphores() const;
        VkSemaphore* GetComputeAvailableSemaphores();
        VkSemaphore* GetComputeFinishedSemaphores();
        const std::vector<VkFence>* GetInFlightFences() const;
        std::vector<VkFence>* GetImagesInFlight();
    };
}
