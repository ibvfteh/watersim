#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class SwapChain;
class RenderPass;
class ImageView;

class Framebuffer
{
public:
    Framebuffer(const Framebuffer &) = delete;
    Framebuffer &operator=(const Framebuffer &) = delete;
    Framebuffer &operator=(Framebuffer &&) = delete;

    explicit Framebuffer(
        const std::vector<VkImageView> &attachments,
        const SwapChain &swapChain,
        const RenderPass &renderPass);
    Framebuffer(Framebuffer &&other) noexcept;
    ~Framebuffer();

    VkFramebuffer GetFramebuffer() const;

private:
    VkFramebuffer framebuffer;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun