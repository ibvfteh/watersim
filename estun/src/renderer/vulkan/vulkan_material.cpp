#include "renderer/vulkan/vulkan_material.h"
#include "renderer/vulkan/material/vulkan_material_pool.h"
#include "renderer/vulkan/material/vulkan_pipeline.h"


namespace estun
{
    VulkanMaterial::VulkanMaterial()
    {
        descriptorSets = new VulkanDescriptorSets(); 
    }

    VulkanMaterial::~VulkanMaterial()
    {
        DeletePipeline();
        delete descriptorSets;
        delete descriptorPool;
    }

    void VulkanMaterial::CreatePipeline(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
    {
        pipeline = VulkanMaterialPoolLocator::GetPipeline(vertexShaderPath, fragmentShaderPath, descriptorSets);
    }

    void VulkanMaterial::CreateTexture(const std::string& texturePath)
    {
        textureImage = VulkanMaterialPoolLocator::GetTextureImage(texturePath);
        textureImageView = VulkanMaterialPoolLocator::GetTextureImageView(texturePath);
        textureSampler = VulkanMaterialPoolLocator::GetTextureSampler(texturePath);
    }

    void VulkanMaterial::CreateDescriptorSets(VulkanUniformBuffer* uniformBuffer)
    {
    	descriptorPool = new VulkanDescriptorPool();
	    descriptorSets->CreateDescriptorSets(
	            descriptorPool->GetDescriptorPool(), 
	            uniformBuffer->GetUniformBuffers(),
	            textureImageView->GetTextureImageView(),
	            textureSampler->GetTextureSampler());
    }

    void VulkanMaterial::DeletePipeline()
    {
        pipeline = nullptr;
    }

    std::shared_ptr<VulkanGraphicsPipeline> VulkanMaterial::GetPipeline() const
    {
        return pipeline;
    }

    VulkanDescriptorSets* VulkanMaterial::GetDescriptorSets() 
    {
        return descriptorSets;
    }
}
