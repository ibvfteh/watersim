#pragma once

#include <vulkan/vulkan.h>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

enum CommandPoolType
{
    Graphics = 0,
    Compute = 1,
    Transfer = 2,
};

class CommandPool
{
private:
    VkCommandPool commandPool;

public:
    CommandPool(const CommandPool &) = delete;
    CommandPool(CommandPool &&) = delete;

    CommandPool &operator=(const CommandPool &) = delete;
    CommandPool &operator=(CommandPool &&) = delete;

    CommandPool(CommandPoolType type);
    ~CommandPool();

    VkCommandPool GetCommandPool() const;

    void Reset();
};

class CommandPoolLocator
{
public:
    static CommandPool &GetGraphicsPool() { return *currGraphicsCommandPool; };
    static CommandPool &GetComputePool() { return *currComputeCommandPool; };
    static CommandPool &GetTransferPool() { return *currTransferCommandPool; };

    static void Provide(
        CommandPool *graphicsCommandPool,
        CommandPool *computeCommandPool,
        CommandPool *transferCommandPool)
    {
        currGraphicsCommandPool = graphicsCommandPool;
        currComputeCommandPool = computeCommandPool;
        currGraphicsCommandPool = currTransferCommandPool;
    };

private:
    static CommandPool *currGraphicsCommandPool;
    static CommandPool *currComputeCommandPool;
    static CommandPool *currTransferCommandPool;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun
