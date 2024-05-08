#include "chunkAsset.hpp"
#include "block.hpp"

void initChunkRenderInfo(ChunkRenderInfo* info) {
    info->numVertices = 0;
    glGenBuffers(1, &info->vertexBuffer);
}

void uploadChunkMesh(std::size_t numVertices, const ChunkMeshVertex* vertices, ChunkRenderInfo* info) {
    glBindBuffer(GL_ARRAY_BUFFER, info->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(BlockTypeIdent), vertices, GL_STATIC_DRAW);

    info->numVertices = numVertices;
}