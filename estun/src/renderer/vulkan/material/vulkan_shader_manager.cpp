#include "renderer/vulkan/material/vulkan_shader_manager.h"
#include "renderer/vulkan/context/vulkan_device.h"

#include <fstream>

namespace estun
{
    ShaderManager* ShaderManager::instance = nullptr;

    ShaderManager* ShaderManager::GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new ShaderManager();
        }
        return instance;
    }

    VkShaderModule ShaderManager::GetShaderModule(const std::string& filename)
    {
        if (loadedShaders.find(filename) == loadedShaders.end())
        {
            loadedShaders[filename] = CreateShaderModule(ReadFile(filename));
        }
        return loadedShaders[filename];
    }

    VkShaderModule ShaderManager::CreateShaderModule(const std::vector<char>& code)
    {

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(*VulkanDeviceLocator::GetLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            ES_CORE_CRITICAL("Failed to create shader module");
        }

        return shaderModule;   
    }

    std::vector<char> ShaderManager::ReadFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            ES_CORE_CRITICAL("Failed to open file {0}", filename);
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    ShaderManager::~ShaderManager()
    {
        CleanUp();
    }

    void ShaderManager::CleanUp()
    {
        for (const auto& shader : loadedShaders)
        {
            vkDestroyShaderModule(*VulkanDeviceLocator::GetLogicalDevice(), shader.second, nullptr);
        }
        loadedShaders.clear();
    }
}
