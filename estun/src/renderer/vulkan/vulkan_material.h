#pragma once

#include "renderer/vulkan/material/vulkan_descriptor_pool.h"
#include "renderer/vulkan/material/vulkan_descriptor_sets.h"
#include "renderer/vulkan/material/vulkan_texture_image.h"
#include "renderer/vulkan/material/vulkan_texture_sampler.h"
#include "renderer/vulkan/material/vulkan_texture_image_view.h"
#include "renderer/vulkan/material/vulkan_pipeline.h"

#include "renderer/vulkan/buffers/vulkan_uniform_buffer.h"

namespace estun
{
    class VulkanMaterial
    {
    private:
		std::shared_ptr<VulkanGraphicsPipeline> pipeline = nullptr;
		std::shared_ptr<VulkanTextureImage>     textureImage = nullptr;
		std::shared_ptr<VulkanTextureImageView> textureImageView = nullptr;
		std::shared_ptr<VulkanTextureSampler>   textureSampler = nullptr;

		VulkanDescriptorSets*  descriptorSets  = nullptr;
		VulkanDescriptorPool* descriptorPool = nullptr;
    public:
        VulkanMaterial();
        ~VulkanMaterial();

        void CreatePipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
        void CreateTexture(const std::string& texturePath);
        void CreateDescriptorSets(VulkanUniformBuffer* uniformBuffer);

        void DeletePipeline();

        std::shared_ptr<VulkanGraphicsPipeline> GetPipeline() const;
        VulkanDescriptorSets*                   GetDescriptorSets();
    };
}
