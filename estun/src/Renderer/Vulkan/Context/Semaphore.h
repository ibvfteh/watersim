#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class Semaphore 
{
public:
    Semaphore(const Semaphore &) = delete;
    Semaphore &operator=(const Semaphore &) = delete;
    Semaphore &operator=(Semaphore &&) = delete;

    explicit Semaphore();
    Semaphore(Semaphore &&other) noexcept;
    ~Semaphore();

    VkSemaphore GetSemaphore() const;

private:
    VkSemaphore semaphore;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun