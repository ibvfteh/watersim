#include "renderer/vulkan/material/vulkan_material_pool.h"

namespace estun
{
    VulkanMaterialPool::VulkanMaterialPool()
    {
        for (const auto& pipeline : pipelinePool)
        {
            delete pipeline.second;
            vertexShaderPath = 
            fragmentShaderPath = 
            pipeline.second = std::make_shared<VulkanGraphicsPipeline>(
			    vertexShaderPath,
		    	fragmentShaderPath,
			    descriptorSetLayout);
        }
    }

    VulkanMaterialPool::~VulkanMaterialPool()
    {
        CleanUp();
    }

	void VulkanMaterialPool::CleanUp()
    {
    	std::vector<std::string> textureToErase;
    	for (const auto& texture : textureImagePool)
	    {
		    if (texture.second.use_count() == 1)
		    {
			    textureToErase.push_back(texture.first);
		    }
	    }

    	for (const auto& textureKey : textureToErase)
    	{
		    textureImagePool.erase(textureKey);
		    textureImageViewPool.erase(textureKey);
		    textureSamplerPool.erase(textureKey);
	    }

	    std::vector<std::string> pipelineToErase;
	    for (const auto& pipeline : pipelinePool)
	    {
		    if (pipeline.second.use_count() == 1)
		    {
			    pipelineToErase.push_back(pipeline.first);
	    	}
	    }

	    for (const auto& pipelineKey : pipelineToErase)
	    {
		    pipelinePool.erase(pipelineKey);
    	}
    }
		
	void VulkanMaterialPool::RebuildPipelines()
    {

    }

    std::shared_ptr<VulkanGraphicsPipeline> VulkanMaterialPool::GetPipeline(
				const std::string& vertexShaderPath,
				const std::string& fragmentShaderPath,
				VkDescriptorSetLayout* descriptorSetLayout)
    {
        const std::string keyString = vertexShaderPath + "|" + fragmentShaderPath;
	    if (pipelinePool.find(keyString) == pipelinePool.end())
	    {
	    	pipelinePool[keyString] = std::make_shared<VulkanGraphicsPipeline>(
			    vertexShaderPath,
		    	fragmentShaderPath,
			    descriptorSetLayout);
		    ES_CORE_INFO("Pipeline loaded and created");
	    }
	    return pipelinePool[keyString];
    }

	std::shared_ptr<VulkanTextureImage> VulkanMaterialPool::GetTextureImage(const std::string& texturePath)
    {
       if (textureImagePool.find(texturePath) == textureImagePool.end())
       {
           textureImagePool[texturePath] = std::make_shared<VulkanTextureImage>(texturePath);
           ES_CORE_INFO("Texture image loaded and created");
       }
       return textureImagePool[texturePath];
    }

	std::shared_ptr<VulkanTextureSampler> VulkanMaterialPool::GetTextureSampler(const std::string& texturePath)
    {
       if (textureSamplerPool.find(texturePath) == textureSamplerPool.end())
       {
           textureSamplerPool[texturePath] = std::make_shared<VulkanTextureSampler>(texturePath);
           ES_CORE_INFO("Texture image view loaded and created");
       }
       return textureSamplerPool[texturePath];
    }

	std::shared_ptr<VulkanTextureImageView> VulkanMaterialPool::GetTextureImageView(const std::string& texturePath)
    {
       if (textureImageViewPool.find(texturePath) == textureImageViewPool.end())
       {
           textureImageViewPool[texturePath] = std::make_shared<VulkanTextureImageView>(texturePath);
           ES_CORE_INFO("Texture image view loaded and created");
       }
       return textureImageViewPool[texturePath];
    }
}
