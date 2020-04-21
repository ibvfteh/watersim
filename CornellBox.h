
#pragma once

#include "Renderer/Vulkan/Model.h"
#include <vector>

class CornellBox
{
public:
    static Estun::Renderer::Vulkan::Model CreateCornellBox(const float scale);

    static void Create(
        float scale,
        std::vector<Estun::Renderer::Vulkan::Vertex> &vertices,
        std::vector<uint32_t> &indices,
        std::vector<Estun::Renderer::Vulkan::Material> &materials);
};
