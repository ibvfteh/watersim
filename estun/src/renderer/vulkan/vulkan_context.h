#pragma once

#include <optional>
#include <set>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "core/core.h"

#include "renderer/vulkan/context/vulkan_instance.h"
#include "renderer/vulkan/context/vulkan_surface.h"
#include "renderer/vulkan/context/vulkan_semaphores.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include "renderer/vulkan/swap_chain/vulkan_command_buffers.h"
#include "renderer/vulkan/swap_chain/vulkan_command_pool.h"
#include "renderer/vulkan/swap_chain/vulkan_framebuffers.h"
#include "renderer/vulkan/swap_chain/vulkan_image_view.h"
#include "renderer/vulkan/swap_chain/vulkan_render_pass.h"
#include "renderer/vulkan/swap_chain/vulkan_resources.h"
#include "renderer/vulkan/swap_chain/vulkan_swap_chain.h"


#include "renderer/vulkan/vulkan_material.h"
#include "renderer/vulkan/material/vulkan_material_pool.h"

namespace estun 
{
    struct GameInfo
    {
        std::string name;
        int version;
        int width;
        int height;

        GameInfo(std::string name, int version, int width, int height)
        {
            this->name = name;
            this->version = version;
            this->width = width;
            this->height = height;
        };
    };

	class VulkanContext 
	{
	public:
		VulkanContext(GLFWwindow* windowHandle, GameInfo* appGameInfo);
        ~VulkanContext();

		void SwapBuffers();
        void Submit(VulkanVertexBuffer* vbo, VulkanIndexBuffer* ibo, VulkanMaterial* material);
        void PrepareFrame();
        void SubmitFrame();
        void ResizeFramebuffers();
        void FreeComandBuffers();
        void EndDraw();

        VulkanRenderPass* GetRenderPass() { return renderPass; }
        VulkanCommandPool* GetCommandPool() { return commandPool; }
        VulkanImageView* GetImageView() { return imageView; }
        VulkanSwapChain* GetSwapChain() { return swapChain; }
		VkPolygonMode GetPolygonMode() { return polygonMode; }
        uint32_t GetCurrentImage() { return imageIndex; }
	private:
		void Init(GLFWwindow* windowHandle);
        void Shutdown();

		void CleanUpSwapChain();
        void RecreateSwapChain();;

    private:
        VulkanInstance*          instance = nullptr;
        VulkanSurface*           surface = nullptr;
        VulkanDevice*            device = nullptr;
        VulkanSwapChain*         swapChain = nullptr;
        VulkanImageView*         imageView = nullptr;
        VulkanRenderPass*        renderPass = nullptr;
        VulkanCommandPool*       commandPool = nullptr;
        VulkanColorResources*    colorResources = nullptr;
        VulkanDepthResources*    depthResources = nullptr;
        VulkanFramebuffers*      framebuffers = nullptr;
        VulkanCommandBuffers*    commandBuffers = nullptr;
        VulkanSemaphoresManager* semaphores = nullptr;
        // TODO VK_POLYGON_MODE_LINE
        VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;

        GameInfo* gameInfo;

        uint32_t imageIndex;
        uint32_t currentFrame;
        uint32_t maxFramesInFlight = 2;

        bool framebufferResized = false;
    };

    class VulkanContextLocator
    {
    public:
        static VulkanContext* GetContext() 
        { 
            if (currContext == nullptr) 
            {
                ES_CORE_ASSERT("Failed to request vulkan context");
            } 
            return currContext; 
        }

        static void Provide(VulkanContext* context) { currContext = context; } 
    private:
        static VulkanContext* currContext;
    };
}
