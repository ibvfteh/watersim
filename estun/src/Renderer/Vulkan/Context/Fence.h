#pragma once

#include <vulkan/vulkan.h>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Fence
{
public:
    Fence(const Fence &) = delete;
    Fence &operator=(const Fence &) = delete;
    Fence &operator=(Fence &&) = delete;

    explicit Fence(bool signaled);
    Fence(Fence &&other) noexcept;
    ~Fence();

    const VkFence &GetFence() const;

    void Reset();
    void Wait(uint64_t timeout) const;

private:
    VkFence fence{};
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun