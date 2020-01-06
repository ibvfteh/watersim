#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>


namespace estun
{
    struct UniformBufferObject
    {    
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

    class VulkanUniformBuffer
    {
    private:
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> mappedMemory;
        
        UniformBufferObject ubo; 
    public:
        VulkanUniformBuffer();
        ~VulkanUniformBuffer();

        void Update(UniformBufferObject& newUbo);
        std::vector<VkBuffer>* GetUniformBuffers();
        std::vector<VkDeviceMemory>* GetUniformBuffersMemory();
    };
}
