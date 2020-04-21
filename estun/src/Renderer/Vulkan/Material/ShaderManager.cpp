#include "Renderer/Vulkan/Material/ShaderManager.h"
#include "Renderer/Vulkan/Context/Device.h"

#include <fstream>

    Estun::Renderer::Vulkan::ShaderManager* Estun::Renderer::Vulkan::ShaderManager::instance = nullptr;

    Estun::Renderer::Vulkan::ShaderManager* Estun::Renderer::Vulkan::ShaderManager::GetInstance()
    {
        if (instance == nullptr)
        {
            instance = new ShaderManager();
        }
        return instance;
    }

    VkShaderModule Estun::Renderer::Vulkan::ShaderManager::GetShaderModule(const std::string& filename)
    {
        if (loadedShaders.find(filename) == loadedShaders.end())
        {
            loadedShaders[filename] = CreateShaderModule(ReadFile(filename));
        }
        return loadedShaders[filename];
    }

    VkShaderModule Estun::Renderer::Vulkan::ShaderManager::CreateShaderModule(const std::vector<char>& code)
    {

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        VK_CHECK_RESULT(vkCreateShaderModule(DeviceLocator::GetLogicalDevice(), &createInfo, nullptr, &shaderModule), "Failed to create shader module");

        return shaderModule;   
    }

    std::vector<char> Estun::Renderer::Vulkan::ShaderManager::ReadFile(const std::string& filename)
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

    Estun::Renderer::Vulkan::ShaderManager::~ShaderManager()
    {
        CleanUp();
    }

    void Estun::Renderer::Vulkan::ShaderManager::CleanUp()
    {
        for (const auto& shader : loadedShaders)
        {
            vkDestroyShaderModule(DeviceLocator::GetLogicalDevice(), shader.second, nullptr);
        }
        loadedShaders.clear();
    }

