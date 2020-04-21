#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

namespace Estun
{

namespace Renderer
{

namespace Vulkan
{

class ShaderModule final
{
public:
    ShaderModule(const ShaderModule &) = delete;
    ShaderModule(ShaderModule &&) = delete;
    ShaderModule &operator=(const ShaderModule &) = delete;
    ShaderModule &operator=(ShaderModule &&) = delete;

    ShaderModule(const std::string &filename);
    ShaderModule(const std::vector<char> &code);
    ~ShaderModule();

    VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

    VkShaderModule GetModule() const;
private:
    static std::vector<char> ReadFile(const std::string &filename);

    VkShaderModule shaderModule_;
};

} // namespace Vulkan

} // namespace Renderer

} // namespace Estun