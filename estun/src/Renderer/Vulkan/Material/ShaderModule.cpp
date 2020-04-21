#include "Renderer/Vulkan/Material/ShaderModule.h"
#include "Renderer/Vulkan/Context/Device.h"
#include <fstream>

Estun::Renderer::Vulkan::ShaderModule::ShaderModule(const std::string& filename) :
	ShaderModule(ReadFile(filename))
{
}

Estun::Renderer::Vulkan::ShaderModule::ShaderModule(const std::vector<char>& code) 
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VK_CHECK_RESULT(vkCreateShaderModule(DeviceLocator::GetLogicalDevice(), &createInfo, nullptr, &shaderModule_), "create shader module");
}

Estun::Renderer::Vulkan::ShaderModule::~ShaderModule()
{
	if (shaderModule_ != nullptr)
	{
		vkDestroyShaderModule(DeviceLocator::GetLogicalDevice(), shaderModule_, nullptr);
		shaderModule_ = nullptr;
	}
}

VkPipelineShaderStageCreateInfo Estun::Renderer::Vulkan::ShaderModule::CreateShaderStage(VkShaderStageFlagBits stage) const
{
	VkPipelineShaderStageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = stage;
	createInfo.module = shaderModule_;
	createInfo.pName = "main";

	return createInfo;
}

std::vector<char> Estun::Renderer::Vulkan::ShaderModule::ReadFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		ES_CORE_ASSERT("failed to open file '" + filename + "'");
	}

	const auto fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}
