#include "assetsInit.hpp"
#include "glHelpers/shader/shader.hpp"
#include "render/chunkRender.hpp"

#include <GL/gl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

static GLuint vertexArray;
static GLuint blockTextureArray;

void initRenderAssets(const RenderAssetsInitInfo& info) {
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

    chunkShader = initShaders(info.chunkShaderSource);

    chunkViewProjectionLocation = glGetUniformLocation(chunkShader, "viewProjection");
    chunkPositionLocation = glGetUniformLocation(chunkShader, "chunkPosition");
    chunkBlockTextureArrayLocation = glGetUniformLocation(chunkShader, "blockTextureArray");

    glGenTextures(1, &blockTextureArray);

    glBindTexture(GL_TEXTURE_2D_ARRAY, blockTextureArray);

    // Allocate the storage && Upload pixel data.
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, ((GLuint)floorf(log2f(16.0f))) + 1, GL_RGBA8, 16, 16, info.numBlockTextureArrayLayers);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 16, 16, info.numBlockTextureArrayLayers, GL_RGBA, GL_UNSIGNED_BYTE, info.blockTextureArrayImagePixels.data());

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}