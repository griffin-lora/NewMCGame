#include "chunkInit.hpp"
#include "render/chunkRender.hpp"

void initChunkRenderInfo(ChunkRenderInfo* info) {
    info->numVertices = 0;
    glGenBuffers(1, &info->vertexBuffer);
}