#include "renderer/context/dynamic_functions.h"
#include "renderer/context/device.h"
#include <string>

namespace
{

    template <class Func>
    Func GetProcedure(const char *const name)
    {
        const auto func = reinterpret_cast<Func>(vkGetDeviceProcAddr(estun::DeviceLocator::GetLogicalDevice(), name));
        if (func == nullptr)
        {
            ES_CORE_ASSERT(std::string("failed to get address of '") + name + std::string("'"));
        }

        return func;
    }

} // namespace

estun::DynamicFunctions::DynamicFunctions()
    : vkCreateAccelerationStructureNV(GetProcedure<PFN_vkCreateAccelerationStructureNV>("vkCreateAccelerationStructureNV")),
      vkDestroyAccelerationStructureNV(GetProcedure<PFN_vkDestroyAccelerationStructureNV>("vkDestroyAccelerationStructureNV")),
      vkGetAccelerationStructureMemoryRequirementsNV(GetProcedure<PFN_vkGetAccelerationStructureMemoryRequirementsNV>("vkGetAccelerationStructureMemoryRequirementsNV")),
      vkBindAccelerationStructureMemoryNV(GetProcedure<PFN_vkBindAccelerationStructureMemoryNV>("vkBindAccelerationStructureMemoryNV")),
      vkCmdBuildAccelerationStructureNV(GetProcedure<PFN_vkCmdBuildAccelerationStructureNV>("vkCmdBuildAccelerationStructureNV")),
      vkCmdCopyAccelerationStructureNV(GetProcedure<PFN_vkCmdCopyAccelerationStructureNV>("vkCmdCopyAccelerationStructureNV")),
      vkCmdTraceRaysNV(GetProcedure<PFN_vkCmdTraceRaysNV>("vkCmdTraceRaysNV")),
      vkCreateRayTracingPipelinesNV(GetProcedure<PFN_vkCreateRayTracingPipelinesNV>("vkCreateRayTracingPipelinesNV")),
      vkGetRayTracingShaderGroupHandlesNV(GetProcedure<PFN_vkGetRayTracingShaderGroupHandlesNV>("vkGetRayTracingShaderGroupHandlesNV")),
      vkGetAccelerationStructureHandleNV(GetProcedure<PFN_vkGetAccelerationStructureHandleNV>("vkGetAccelerationStructureHandleNV")),
      vkCmdWriteAccelerationStructuresPropertiesNV(GetProcedure<PFN_vkCmdWriteAccelerationStructuresPropertiesNV>("vkCmdWriteAccelerationStructuresPropertiesNV")),
      vkCompileDeferredNV(GetProcedure<PFN_vkCompileDeferredNV>("vkCompileDeferredNV"))
{
}

estun::DynamicFunctions::~DynamicFunctions()
{
}

estun::DynamicFunctions *estun::FunctionsLocator::funcs_ = nullptr;