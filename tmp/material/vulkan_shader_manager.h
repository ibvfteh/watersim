#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

namespace estun
{
    class ShaderManager
    {
    private:
        static ShaderManager* instance;

        std::unordered_map<std::string, VkShaderModule> loadedShaders;
    
    public:
        ~ShaderManager();

        void CleanUp();

        static ShaderManager* GetInstance();

        VkShaderModule GetShaderModule(const std::vector<char>& code);
    private:
        ShaderManager() = default;

        static std::vector<char> ReadFile(const std::string& filename);

        static VkShaderModule CreateShaderModule(const std::vector<char>& code);
    }
}
