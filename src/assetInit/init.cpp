#include "init.hpp"
#include "glHelpers/shader/shader.hpp"
#include "render/chunkRender.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

static GLuint vertexArray;
static GLuint blockTextureArray;

void initRenderAssets() {
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
    
    // TODO: Actually load block textures
}