#include "init.hpp"
#include "glHelpers/shader/shader.hpp"
#include "glHelpers/utils/logger/logger.hpp"
#include "render/chunkRender.hpp"
#include "glHelpers/utils/getEnvVar/getEnvVar.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "glHelpers/utils/stb_image.h"

#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>

static GLuint vertexArray;
static GLuint blockTextureArray;

#include <filesystem>
namespace fs = std::filesystem;

void initRenderAssets() {
    Logger logger("Init Render Assets", Logger::FGColors::GREEN);

    // Technically some things in here combine asset/ + assetUpload/ + assetInit/ functionality, but its fine
    // Its fine because we are only doing this once at the start of the program

    // Set some OpenGL settings
    // Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

    // Set background color
    glClearColor(.65f, .89f, 1.f, 1.f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    chunkShader = loadShaders("chunk/");

    chunkViewProjectionLocation = glGetUniformLocation(chunkShader, "viewProjection");
    chunkPositionLocation = glGetUniformLocation(chunkShader, "chunkPosition");
    chunkBlockTextureArrayLocation = glGetUniformLocation(chunkShader, "blockTextureArray");

    glGenTextures(1, &blockTextureArray);

    glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    auto texturesDir = fs::absolute(getEnvironmentVar("TEXTURES_FOLDER"));

    fs::path packDir = texturesDir / "world" / "base";

    std::vector<std::uint8_t> blockTextureArrayImage;

    std::size_t numTextures = 3;

    std::string imageNames[3] = {
        "grass_side.png",
        "grass_top.png",
        "dirt.png"
    };

    for (size_t i = 0; i < numTextures; i++) {
        const std::string& imageName = imageNames[i];
        std::string imagePath = packDir / imageName;

        int textureWidth;
        int textureHeight;
        std::uint8_t* imagePixels = stbi_load(imagePath.c_str(), &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);
        
        // If we failed to open the image
        if (imagePixels == nullptr) {
            logger.error("Image " + imagePath + " failed to load.");
            
            stbi_image_free(imagePixels);
            return;
        }

        // Only supports 16x16 images right now
        if (textureWidth != 16 || textureHeight != 16) {
            logger.error("Image " + imagePath + " is NOT 16x16. Skipping.");

            stbi_image_free(imagePixels);
            return;
        }

        blockTextureArrayImage.insert(blockTextureArrayImage.end(), imagePixels, imagePixels + (textureWidth * textureHeight * 4));

        stbi_image_free(imagePixels);

        logger.info("Loaded image with path " + imagePath);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);

    // Allocate the storage && Upload pixel data.
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 16, 16, numTextures);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, numTextures, GL_RGBA, GL_UNSIGNED_BYTE, blockTextureArrayImage.data());
}