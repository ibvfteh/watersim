#include "renderer/vulkan/vulkan_context.h"
#include <iostream>

namespace estun {
    VulkanContext* VulkanContextLocator::context = nullptr;
	VulkanContext::VulkanContext(GLFWwindow* windowHandle, GameInfo* gameInfo)
	{
        this->gameInfo = gameInfo;
        if (!windowHandle)
        {
		    ES_CORE_ASSERT("Window handle is null!");
        }
		Init(windowHandle);
	}

    VulkanContext::~VulkanContext()
    {
        Shutdown();
    }
	
	void VulkanContext::Init(GLFWwindow* windowHandle)
	{
        ES_CORE_INFO("start vulkan init");
	    instance       = new VulkanInstance(gameInfo->name.c_str(), gameInfo->version);
        ES_CORE_INFO("instance done");
        surface        = new VulkanSurface(instance, windowHandle);
        ES_CORE_INFO("surface done");
        device         = new VulkanDevice(instance, surface);
        ES_CORE_INFO("device done");
        VulkanDeviceLocator::Provide(device);

        swapChain      = new VulkanSwapChain(surface, device->GetQueueFamilyIndices(), gameInfo->width, gameInfo->height);
        ES_CORE_INFO("swap chain done");
        imageView      = new VulkanImageView(swapChain);
        ES_CORE_INFO("image view done");
        renderPass     = new VulkanRenderPass(*swapChain->GetSwapChainImageFormat(), device->GetMsaaSamples());
        ES_CORE_INFO("render pass done");
        
        commandPool    = new VulkanCommandPool();
        ES_CORE_INFO("command pool done");
        colorResources = new VulkanColorResources(swapChain, device->GetMsaaSamples());
        ES_CORE_INFO("color resources done");
        depthResources = new VulkanDepthResources(swapChain->GetSwapChainExtent(), device->GetMsaaSamples());
        ES_CORE_INFO("depth resources done");
        framebuffers   = new VulkanFramebuffers(imageView, renderPass, swapChain->GetSwapChainExtent(), depthResources->GetDepthImageView(), colorResources->GetColorImageView());
        ES_CORE_INFO("frame buffers done");
       
        commandBuffers = new VulkanCommandBuffers(commandPool, swapChain->GetImageCount());
        ES_CORE_INFO("command buffers done");
        semaphores     = new VulkanSemaphoresManager(swapChain->GetImageCount());
        ES_CORE_INFO("semaphores done");
    }  
    
    void VulkanContext::Shutdown()
    {
        delete semaphores;
        CleanUpSwapChain();
        delete device;
        VulkanDeviceLocator::Provide(nullptr);
        surface->Delete(instance);
        delete surface;
        delete instance;
    }

    void VulkanContext::RecreateSwapChain()
    {
        vkDeviceWaitIdle(*device->GetLogicalDevice());

        CleanUpSwapChain();

        swapChain      = new VulkanSwapChain(surface, device->GetQueueFamilyIndices(), gameInfo->width, gameInfo->height);
        imageView      = new VulkanImageView(swapChain);
        renderPass     = new VulkanRenderPass(*swapChain->GetSwapChainImageFormat(), device->GetMsaaSamples());
        
        colorResources = new VulkanColorResources(swapChain, device->GetMsaaSamples());
        depthResources = new VulkanDepthResources(swapChain->GetSwapChainExtent(), device->GetMsaaSamples());
        framebuffers   = new VulkanFramebuffers(imageView, renderPass, swapChain->GetSwapChainExtent(), depthResources->GetDepthImageView(), colorResources->GetColorImageView());
        
        commandBuffers = new VulkanCommandBuffers(commandPool, swapChain->GetImageCount());
    }

    void VulkanContext::CleanUpSwapChain()
    {
        delete commandBuffers;
        delete depthResources;
        delete colorResources;
        delete framebuffers;
        delete commandPool;
        delete renderPass;
        delete imageView;
        delete swapChain;
    }
/*
    void VulkanContext::SwapBuffers()
    {
        //glfwSwapBuffers(m_WindowHandle->m_Window);
    }

	void VulkanContext::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
        //int width = 0, height = 0;
        //glfwGetFramebufferSize(window, &width, &height);
        //while (width == 0 || height == 0) {
        //    glfwGetFramebufferSize(window, &width, &height);
        //    glfwWaitEvents();
        //}
		//glViewport(x, y, width, height);
	}

	void VulkanContext::SetClearColor(const glm::vec4& color)
	{
		//glClearColor(color.r, color.g, color.b, color.a);
	}

	void VulkanContext::Clear()
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void VulkanContext::DrawIndexed(const ref<VertexArray>& vertexArray)
	{
		//glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

*/
}
