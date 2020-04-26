#include "renderer/ray_tracing/base_acceleration_structure.h"
#include "renderer/context/device.h"

#undef MemoryBarrier

estun::BaseAccelerationStructure::BaseAccelerationStructure(const VkAccelerationStructureCreateInfoKHR &createInfo)
    : allowUpdate_(createInfo.flags & VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR)
{
    VK_CHECK_RESULT(vkCreateAccelerationStructureKHR(DeviceLocator::GetLogicalDevice(), &createInfo, nullptr, &accelerationStructure_), "create acceleration structure");
}

estun::BaseAccelerationStructure::BaseAccelerationStructure(BaseAccelerationStructure &&other) noexcept
    : allowUpdate_(other.allowUpdate_),
      accelerationStructure_(other.accelerationStructure_)
{
    other.accelerationStructure_ = nullptr;
}

estun::BaseAccelerationStructure::~BaseAccelerationStructure()
{
    if (accelerationStructure_ != nullptr)
    {
        vkDestroyAccelerationStructureKHR(DeviceLocator::GetLogicalDevice(), accelerationStructure_, nullptr);
        accelerationStructure_ = nullptr;
    }
}

estun::MemoryRequirements estun::BaseAccelerationStructure::GetMemoryRequirements() const
{
    VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo{};
    memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_KHR;
    memoryRequirementsInfo.pNext = nullptr;
    memoryRequirementsInfo.accelerationStructure = accelerationStructure_;

    // If the descriptor already contains the geometry info, so we can directly compute the estimated size and required scratch memory.
    VkMemoryRequirements2 memoryRequirements = {};
    memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_OBJECT_KHR;
    vkGetAccelerationStructureMemoryRequirementsNV(DeviceLocator::GetLogicalDevice(), &memoryRequirementsInfo, &memoryRequirements);
    const auto resultRequirements = memoryRequirements.memoryRequirements;

    memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_BUILD_SCRATCH_KHR;
    vkGetAccelerationStructureMemoryRequirementsNV(DeviceLocator::GetLogicalDevice(), &memoryRequirementsInfo, &memoryRequirements);
    const auto buildRequirements = memoryRequirements.memoryRequirements;

    memoryRequirementsInfo.type = VK_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_TYPE_UPDATE_SCRATCH_KHR;
    vkGetAccelerationStructureMemoryRequirementsNV(DeviceLocator::GetLogicalDevice(), &memoryRequirementsInfo, &memoryRequirements);
    const auto updateRequirements = memoryRequirements.memoryRequirements;

    return {resultRequirements, buildRequirements, updateRequirements};
}

void estun::BaseAccelerationStructure::ASMemoryBarrier(VkCommandBuffer commandBuffer)
{
    // Wait for the builder to complete by setting a barrier on the resulting buffer. This is
    // particularly important as the construction of the top-level hierarchy may be called right
    // afterwards, before executing the command list.
    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
}

VkAccelerationStructureNV estun::BaseAccelerationStructure::GetStructure() const
{
    return accelerationStructure_;
}