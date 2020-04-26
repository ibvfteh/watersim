#pragma once

#include "renderer/common.h"

#include <GLFW/glfw3.h>

#include "core/core.h"

#include "renderer/context/instance.h"
#include "renderer/context/surface.h"
#include "renderer/context/device.h"
#include "renderer/context/swap_chain.h"
#include "renderer/context/command_pool.h"
#include "renderer/context/command_buffers.h"
#include "renderer/context/semaphore.h"
#include "renderer/context/fence.h"
#include "renderer/render.h"

namespace estun
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

    void StartDraw();
    //void EndDraw();
    void SubmitDraw(std::vector<Render> &renders);

    void DrawIndexed(uint32_t indexesSize, uint32_t indexOffset, uint32_t vertexOffset);

    //void CreateRayTracingOutputImage();
    //void DeleteRayTracingOutputImage();

    //void StartRayTracing();
    //void EndRayTracing(ShaderBindingTable &shaderBindingTable);

    SwapChain *GetSwapChain() { return swapChain_.get(); }
    VkSampleCountFlagBits &GetMsaaSamples() { return msaa_; }
    uint32_t GetImageIndex() { return imageIndex_; }

private:
    std::unique_ptr<Instance> instance_;
    std::unique_ptr<Surface> surface_;
    std::unique_ptr<Device> device_;
    std::unique_ptr<SwapChain> swapChain_;
    std::unique_ptr<CommandPool> graphicsCommandPool_;
    std::unique_ptr<CommandPool> computeCommandPool_;
    std::unique_ptr<CommandPool> transferCommandPool_;

    std::vector<Semaphore> imageAvailableSemaphores_;
    std::vector<Semaphore> renderFinishedSemaphores_;
    std::vector<Fence> inFlightFences_;

    // TODO VK_POLYGON_MODE_LINE
    // VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;

    GameInfo *gameInfo_;

    uint32_t imageIndex_;
    bool vsync_;
    uint32_t currentFrame_{};
    bool isWireFrame_;
    bool setWireFrame_;
    VkSampleCountFlagBits msaa_;

    //uint32_t maxFramesInFlight_ = 2;

    //bool framebufferResized = false;
    //bool firstCompute = true;
};

class ContextLocator
{
public:
    static Context *GetContext();
    static SwapChain *GetSwapChain();
    static uint32_t GetImageIndex();

    static void Provide(Context *context) { currContext = context; }

private:
    static Context *currContext;
};

} // namespace estun
