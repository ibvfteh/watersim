#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class ShaderManager
{
private:
    static ShaderManager *instance;

    std::unordered_map<std::string, VkShaderModule> loadedShaders;

public:
    ~ShaderManager();

    void CleanUp();

    static ShaderManager *GetInstance();

    VkShaderModule GetShaderModule(const std::string &filename);

private:
    ShaderManager() = default;

    static std::vector<char> ReadFile(const std::string &filename);

    static VkShaderModule CreateShaderModule(const std::vector<char> &code);
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun