#pragma once
#include <GL/glew.h>
#include <cstddef>
#include <glm/glm.hpp>

struct ChunkRenderInfo {
    std::size_t numVertices = 0;
    GLuint vertexBuffer = 0;
};

void renderChunk(glm::vec3 pos, const ChunkRenderInfo* info);