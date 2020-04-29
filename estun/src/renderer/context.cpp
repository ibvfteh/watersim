#include "renderer/context.h"
#include "renderer/context/image_memory_barier.h"

estun::Context::Context(GLFWwindow *windowHandle, GameInfo *gameInfo)
{
    if (!windowHandle)
    {
        ES_CORE_ASSERT("Window handle is null!");
    }

    gameInfo_ = gameInfo;

    ES_CORE_INFO("Start vulkan init");
    instance_.reset(new Instance(gameInfo_->name_.c_str(), gameInfo_->version_));
    ES_CORE_INFO("Instance done");
    surface_.reset(new Surface(instance_.get(), windowHandle));
    ES_CORE_INFO("Surface done");
    device_.reset(new Device(instance_.get(), surface_.get()));
    DeviceLocator::Provide(device_.get());
    dynamicFunctions_.reset(new DynamicFunctions());
    FunctionsLocator::Provide(dynamicFunctions_.get());
    ES_CORE_INFO("Device done");

    graphicsCommandPool_.reset(new CommandPool(Graphics));
    ES_CORE_INFO("Graphics command pool done");
    computeCommandPool_.reset(new CommandPool(Compute));
    ES_CORE_INFO("Compute command pool done");
    transferCommandPool_.reset(new CommandPool(Transfer));
    ES_CORE_INFO("Transfer command pool done");
    CommandPoolLocator::Provide(graphicsCommandPool_.get(), computeCommandPool_.get(), transferCommandPool_.get());

    msaa_ = VK_SAMPLE_COUNT_1_BIT;
    if (gameInfo_->msaa_)
        msaa_ = device_->GetMsaaSamples();

    CreateSwapChain();
}

estun::Context::~Context()
{
    DeleteSwapChain();

    transferCommandPool_.reset();
    computeCommandPool_.reset();
    graphicsCommandPool_.reset();
    device_.reset();
    surface_.reset();
    instance_.reset();
}

void estun::Context::CreateSwapChain()
{
    swapChain_.reset(new SwapChain(surface_.get(), gameInfo_->width_, gameInfo_->height_, gameInfo_->vsync_));
    ES_CORE_INFO("Swap chain done");

    //colorResources_.reset(new ColorResources(swapChain_.get(), msaa_));
    //ES_CORE_INFO("Color resources done");
    //depthResources_.reset(new DepthResources(swapChain_->GetExtent(), msaa_));
    //ES_CORE_INFO("Depth resources done");

    for (size_t i = 0; i != swapChain_->GetImageViews().size(); ++i)
    {
        imageAvailableSemaphores_.emplace_back();
        renderFinishedSemaphores_.emplace_back();
        inFlightFences_.emplace_back(true);
    }
    ES_CORE_INFO("Semaphores done");
    //CreateRayTracingOutputImage();
    ES_CORE_INFO("RayTracingImages done");
}

void estun::Context::DeleteSwapChain()
{
    renders_.clear();
    //DeleteRayTracingOutputImage();
    inFlightFences_.clear();
    renderFinishedSemaphores_.clear();
    imageAvailableSemaphores_.clear();
    //depthResources_.reset();
    //colorResources_.reset();
    //commandBuffers_.reset();
    swapChain_.reset();
}

void estun::Context::RecreateSwapChain()
{
    device_->WaitIdle();
    DeleteSwapChain();
    CreateSwapChain();
}

std::shared_ptr<estun::Render> estun::Context::CreateRender()
{
    std::shared_ptr<Render> render = std::make_shared<Render>();
    renders_.push_back(render);
    return render;
}

void estun::Context::StartDraw()
{
    const auto noTimeout = std::numeric_limits<uint64_t>::max();

    auto &inFlightFence = inFlightFences_[currentFrame_];
    const auto imageAvailableSemaphore = imageAvailableSemaphores_[currentFrame_].GetSemaphore();

    inFlightFence.Wait(noTimeout);

    auto result = vkAcquireNextImageKHR(device_->GetLogicalDevice(), swapChain_->GetSwapChain(), noTimeout, imageAvailableSemaphore, nullptr, &imageIndex_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        ES_CORE_ASSERT(std::string("Failed to acquire next image (") + std::string(")"));
    }
}

void estun::Context::SubmitDraw()
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const auto imageAvailableSemaphore = imageAvailableSemaphores_[currentFrame_].GetSemaphore();
    const auto renderFinishedSemaphore = renderFinishedSemaphores_[currentFrame_].GetSemaphore();
    auto &inFlightFence = inFlightFences_[currentFrame_];

    std::vector<VkCommandBuffer> commandBuffers;
    for (auto &render : renders_)
    {
        commandBuffers.push_back(render->GetCurrCommandBuffer());
    }
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    inFlightFence.Reset();

    VK_CHECK_RESULT(vkQueueSubmit(device_->GetGraphicsQueue(), 1, &submitInfo, inFlightFence.GetFence()), "submit draw command buffer");

    VkSwapchainKHR swapChains[] = {swapChain_->GetSwapChain()};
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex_;
    presentInfo.pResults = nullptr; // Optional

    auto result = vkQueuePresentKHR(device_->GetPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS)
    {
        ES_CORE_ASSERT(std::string("failed to present next image (") + std::string(")"));
    }

    currentFrame_ = (currentFrame_ + 1) % inFlightFences_.size();
}
/*
void estun::Context::CreateRayTracingOutputImage()
{
    const auto extent = swapChain_->GetExtent();
    const auto format = swapChain_->GetFormat();
    const auto tiling = VK_IMAGE_TILING_OPTIMAL;

    accumulationImage_.reset(new ImageHolder(
        extent.width, extent.height,
        VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_STORAGE_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT));

    outputImage_.reset(new ImageHolder(
        extent.width, extent.height,
        format,
        tiling,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT));
}

void estun::Context::DeleteRayTracingOutputImage()
{
    accumulationImage_.reset();
    outputImage_.reset();
}

void estun::Context::StartRayTracing()
{
    const auto extent = swapChain_->GetExtent();

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    ImageMemoryBarrier::Insert(currCommandBuffer_, accumulationImage_->GetImage().GetImage(), subresourceRange, 0,
                               VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    ImageMemoryBarrier::Insert(currCommandBuffer_, outputImage_->GetImage().GetImage(), subresourceRange, 0,
                               VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
}

void estun::Context::EndRayTracing(ShaderBindingTable &shaderBindingTable)
{
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    vkCmdTraceRaysNV(currCommandBuffer_,
                     shaderBindingTable.GetBuffer().GetBuffer(), shaderBindingTable.RayGenOffset(),
                     shaderBindingTable.GetBuffer().GetBuffer(), shaderBindingTable.MissOffset(), shaderBindingTable.MissEntrySize(),
                     shaderBindingTable.GetBuffer().GetBuffer(), shaderBindingTable.HitGroupOffset(), shaderBindingTable.HitGroupEntrySize(),
                     nullptr, 0, 0,
                     swapChain_->GetExtent().width, swapChain_->GetExtent().height, 1);

    ImageMemoryBarrier::Insert(currCommandBuffer_, outputImage_->GetImage().GetImage(), subresourceRange,
                               VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    ImageMemoryBarrier::Insert(currCommandBuffer_, swapChain_->GetImages()[imageIndex_], subresourceRange, 0,
                               VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VkImageCopy copyRegion;
    copyRegion.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copyRegion.srcOffset = {0, 0, 0};
    copyRegion.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copyRegion.dstOffset = {0, 0, 0};
    copyRegion.extent = {swapChain_->GetExtent().width, swapChain_->GetExtent().height, 1};

    vkCmdCopyImage(currCommandBuffer_,
                   outputImage_->GetImage().GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   swapChain_->GetImages()[imageIndex_], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &copyRegion);

    ImageMemoryBarrier::Insert(currCommandBuffer_, swapChain_->GetImages()[imageIndex_], subresourceRange, VK_ACCESS_TRANSFER_WRITE_BIT,
                               0, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}
*/

estun::Context *estun::ContextLocator::currContext = nullptr;

estun::Context *estun::ContextLocator::GetContext()
{
    if (currContext == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan context");
    }
    return currContext;
}

estun::SwapChain *estun::ContextLocator::GetSwapChain()
{
    if (currContext == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan context");
    }
    return currContext->GetSwapChain();
}

uint32_t estun::ContextLocator::GetImageIndex()
{
    if (currContext == nullptr)
    {
        ES_CORE_ASSERT("Failed to request vulkan context");
    }
    return currContext->GetImageIndex();
}