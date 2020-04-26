#pragma once

#include "renderer/common.h"

#include "core/core.h"

#include "renderer/context/command_pool.h"
#include "renderer/context/command_buffers.h"
#include "renderer/context/resources.h"
#include "renderer/context/render_pass.h"
#include "renderer/context/framebuffer.h"
#include "renderer/material/graphics_pipeline.h"
#include "renderer/material/descriptor.h"
#include "renderer/buffers/storage_buffer.h"
#include "renderer/model.h"
//#include "renderer/ray_tracing/ShaderBildingTable.h"
#include "renderer/buffers/buffer.h"
#include "renderer/context/image.h"

#include "renderer/context/image_holder.h"

namespace estun
{

class Render
{
public:
    Render(const Render &) = delete;
    Render(Render &&) = delete;

    Render &operator=(const Render &) = delete;
    Render &operator=(Render &&) = delete;

    Render(bool toDefault = true);
    ~Render();

    void CreateRender();
    void DestroyRender();
    void RecreateRender();

    void StartDrawInCurrent();
    void RecordDrawInCurrent();
    void Bind(std::unique_ptr<Descriptor> &descriptor);
    void Bind(std::unique_ptr<GraphicsPipeline> &pipeline);
    void Bind(std::unique_ptr<VertexBuffer> &vertexBuffer);
    void Bind(std::unique_ptr<IndexBuffer> &indexBuffer);
    void DrawIndexed(uint32_t indexesSize, uint32_t indexOffset, uint32_t vertexOffset);

    //void CreateRayTracingOutputImage();
    //void DeleteRayTracingOutputImage();

    VkCommandBuffer &GetCurrCommandBuffer();
    
/*
    VkFramebuffer &GetFramebuffer(uint32_t index);
    ImageHolder *GetAccumulationImage();
    ImageHolder *GetOutputImage();
    */

private:
    bool toDefault_;
    std::unique_ptr<CommandBuffers> commandBuffers_;
    std::unique_ptr<ColorResources> colorResources_;
    std::unique_ptr<DepthResources> depthResources_;
    std::unique_ptr<ColorResources> colorResolveResources_;
    std::unique_ptr<RenderPass> renderPass_;
    std::shared_ptr<GraphicsPipeline> currPipeline_;
    std::vector<Framebuffer> framebuffers_;

    std::unique_ptr<ImageHolder> accumulationImage_;
    std::unique_ptr<ImageHolder> outputImage_;

    // TODO VK_POLYGON_MODE_LINE
    // VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;

    VkSampleCountFlagBits msaa_;

    VkCommandBuffer currCommandBuffer_;
};

} // namespace estun