#pragma once

#include "renderer/model.h"
#include <vector>

class WaterSurface
{
public:
    static estun::Model CreateSurface(const float scale, const int width, const int height);

    static void Create(
        float scale, const int width, const int height,
        std::vector<estun::Vertex> &vertices,
        std::vector<uint32_t> &indices,
        std::vector<estun::Material> &materials);
};