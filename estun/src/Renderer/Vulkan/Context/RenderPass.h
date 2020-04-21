#pragma once

#include <vulkan/vulkan.h>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
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

    void Begin(Framebuffer farmebuffer);
    void End();

    VkRenderPass GetRenderPass() const;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
