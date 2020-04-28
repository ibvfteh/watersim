#pragma once

#include "renderer/common.h"
#include "includes/glm.h"
#include "renderer/material/descriptable.h"

namespace estun
{

class Buffer;
class DeviceMemory;

struct UniformBufferObject
{
    //alignas(16) glm::mat4 model;
    //alignas(16) glm::mat4 view;
    //alignas(16) glm::mat4 proj;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    //glm::mat4 ModelViewInverse;
    //glm::mat4 ProjectionInverse;
    //float Aperture;
    //float FocusDistance;
    //uint32_t TotalNumberOfSamples;
    //uint32_t NumberOfSamples;
    //uint32_t NumberOfBounces;
    //uint32_t RandomSeed;
    //uint32_t GammaCorrection; // bool
    //uint32_t HasSky;          // bool
};

class UniformBuffer : public Descriptable
{
public:
    UniformBuffer(const UniformBuffer &) = delete;
    UniformBuffer &operator=(const UniformBuffer &) = delete;
    UniformBuffer &operator=(UniformBuffer &&) = delete;

    explicit UniformBuffer();
    UniformBuffer(UniformBuffer &&other) noexcept;
    ~UniformBuffer();

    DescriptableInfo GetInfo() override;

    const Buffer &GetBuffer() const;

    void SetValue(const UniformBufferObject &ubo);

private:
    std::unique_ptr<Buffer> buffer;
    std::unique_ptr<DeviceMemory> memory;
};

} // namespace estun