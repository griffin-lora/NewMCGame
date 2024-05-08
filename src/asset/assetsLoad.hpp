#pragma once

#include "glHelpers/shader/shader.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

struct RenderAssetsInitInfo {
    ShaderSource chunkShaderSource;
    std::size_t numBlockTextureArrayLayers;
    std::vector<std::uint8_t> blockTextureArrayImagePixels;
};

std::optional<RenderAssetsInitInfo> loadRenderAssets();