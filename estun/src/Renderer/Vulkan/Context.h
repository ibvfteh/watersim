#pragma once

#include <optional>
#include <set>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "Core/Core.h"

#include "Renderer/Vulkan/Context/Instance.h"
#include "Renderer/Vulkan/Context/Surface.h"
#include "Renderer/Vulkan/Context/Device.h"
#include "Renderer/Vulkan/Context/SwapChain.h"
#include "Renderer/Vulkan/Context/CommandPool.h"
#include "Renderer/Vulkan/Context/CommandBuffers.h"
#include "Renderer/Vulkan/Context/Resources.h"
#include "Renderer/Vulkan/Context/Semaphore.h"
#include "Renderer/Vulkan/Context/Fence.h"

#include "Renderer/Vulkan/Context/RenderPass.h"
#include "Renderer/Vulkan/Material/GraphicsPipeline.h"
#include "Renderer/Vulkan/Model.h"
//#include "Renderer/Vulkan/RayTracing/ShaderBildingTable.h"
#include "Renderer/Vulkan/Buffers/Buffer.h"
#include "Renderer/Vulkan/Material/Image.h"

#include "Renderer/Vulkan/Context/ImageHolder.h"

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

struct GameInfo
{
    std::string name_;
    Version version_;
    uint32_t width_;
    uint32_t height_;
    bool vsync_;
    bool msaa_;

    GameInfo(std::string name, Version version, int width, int height, bool vsync, bool msaa)
    {
        this->name_ = name;
        this->version_ = version;
        this->width_ = width;
        this->height_ = height;
        this->vsync_ = vsync;
        this->msaa_ = msaa;
    };
};

class Context
{
public:
    Context(const Context &) = delete;
    Context(Context &&) = delete;

    Context &operator=(const Context &) = delete;
    Context &operator=(Context &&) = delete;

    Context(GLFWwindow *windowHandle, GameInfo *gameInfo);
    ~Context();

    void CreateSwapChain();
    void DeleteSwapChain();
    void RecreateSwapChain();
    void DrawFrame(GraphicsPipeline pipeline, RenderPass renderPass);

    void StartDraw();
    void EndDraw();
    void SubmitDraw();

    void DrawIndexed(uint32_t indexesSize, uint32_t indexOffset, uint32_t vertexOffset);

    //void CreateRayTracingOutputImage();
    //void DeleteRayTracingOutputImage();

    //void StartRayTracing();
    //void EndRayTracing(ShaderBindingTable &shaderBindingTable);

    SwapChain *GetSwapChain() { return swapChain_.get(); }
    VkSampleCountFlagBits &GetMsaaSamples() { return msaa_; }
    VkCommandBuffer &GetCurrentCommandBuffer() { return currCommandBuffer_; }
    uint32_t GetImageIndex() { return imageIndex_; }
    ImageHolder* GetAccumulationImage() { return accumulationImage_.get(); }
    ImageHolder* GetOutputImage() { return outputImage_.get(); }

private:
    std::unique_ptr<Instance> instance_;
    std::unique_ptr<Surface> surface_;
    std::unique_ptr<Device> device_;
    std::unique_ptr<SwapChain> swapChain_;
    std::unique_ptr<CommandPool> graphicsCommandPool_;
    std::unique_ptr<CommandPool> computeCommandPool_;
    std::unique_ptr<CommandPool> transferCommandPool_;
    std::unique_ptr<CommandBuffers> commandBuffers_;
    std::unique_ptr<ColorResources> colorResources_;
    std::unique_ptr<DepthResources> depthResources_;
    std::unique_ptr<RenderPass> mainRenderPass_;
    std::vector<Framebuffer> framebuffers_;
    std::vector<Semaphore> imageAvailableSemaphores_;
    std::vector<Semaphore> renderFinishedSemaphores_;
    std::vector<Fence> inFlightFences_;

    std::unique_ptr<ImageHolder> accumulationImage_;
    std::unique_ptr<ImageHolder> outputImage_;

    // TODO VK_POLYGON_MODE_LINE
    // VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;

    GameInfo *gameInfo_;

    uint32_t imageIndex_;
    bool vsync_;
    uint32_t currentFrame_{};
    bool isWireFrame_;
    bool setWireFrame_;
    VkSampleCountFlagBits msaa_;

    VkCommandBuffer currCommandBuffer_;
    //uint32_t maxFramesInFlight_ = 2;

    //bool framebufferResized = false;
    //bool firstCompute = true;
};

class ContextLocator
{
public:
    static Context *GetContext();
    static SwapChain *GetSwapChain();
    static VkCommandBuffer &GetCurrentCommandBuffer();
    static uint32_t GetImageIndex();

    static void Provide(Context *context) { currContext = context; }

private:
    static Context *currContext;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
