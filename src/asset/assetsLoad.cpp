#include "assetsLoad.hpp"
#include "glHelpers/shader/shader.hpp"
#include "glHelpers/utils/getEnvVar/getEnvVar.hpp"
#include "glHelpers/utils/logger/logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "glHelpers/utils/stb_image.h"

#include <vector>

#include <filesystem>
namespace fs = std::filesystem;

std::optional<RenderAssetsInitInfo> loadRenderAssets() {
    Logger logger("Load Render Assets", Logger::FGColors::GREEN);

    ShaderSource chunkShaderSource = loadShaders("chunk/");

    auto texturesDir = fs::absolute(getEnvironmentVar("TEXTURES_FOLDER"));
    fs::path packDir = texturesDir / "world" / "base";

    std::size_t numBlockTextureArrayLayers = 3;
    std::vector<std::uint8_t> blockTextureArrayImagePixels;

    std::string imageNames[3] = {
        "grass_side.png",
        "grass_top.png",
        "dirt.png"
    };

    for (size_t i = 0; i < numBlockTextureArrayLayers; i++) {
        const std::string& imageName = imageNames[i];
        std::string imagePath = packDir / imageName;

        int textureWidth;
        int textureHeight;
        std::uint8_t* imagePixels = stbi_load(imagePath.c_str(), &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);
        
        // If we failed to open the image
        if (imagePixels == nullptr) {
            logger.error("Image " + imagePath + " failed to load.");
            
            stbi_image_free(imagePixels);
            return {};
        }

        // Only supports 16x16 images right now
        if (textureWidth != 16 || textureHeight != 16) {
            logger.error("Image " + imagePath + " is NOT 16x16. Skipping.");

            stbi_image_free(imagePixels);
            return {};
        }

        blockTextureArrayImagePixels.insert(blockTextureArrayImagePixels.end(), imagePixels, imagePixels + (textureWidth * textureHeight * 4));

        stbi_image_free(imagePixels);

        logger.info("Loaded image with path " + imagePath);
    }

    return (RenderAssetsInitInfo) {
        .chunkShaderSource = chunkShaderSource,
        .numBlockTextureArrayLayers = numBlockTextureArrayLayers,
        .blockTextureArrayImagePixels = blockTextureArrayImagePixels
    };
}