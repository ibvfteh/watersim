#pragma once

#include "renderer/common.h"

namespace estun
{

class Framebuffer;

class RenderPass
{
private:
    VkRenderPass renderPass;

public:
    RenderPass(const RenderPass &) = delete;
    RenderPass(RenderPass &&) = delete;

    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass &operator=(RenderPass &&) = delete;

    RenderPass();
    ~RenderPass();

    void Begin(Framebuffer &farmebuffer, VkCommandBuffer &commandBuffer);
    void End(VkCommandBuffer &commandBuffer);

    VkRenderPass GetRenderPass() const;
};

} // namespace estun
