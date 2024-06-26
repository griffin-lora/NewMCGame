#pragma once

#include "asset/chunkMesh.hpp"
#include "render/chunkRender.hpp"

void initChunkRenderInfo(ChunkRenderInfo* info);
void uploadChunkMesh(std::size_t numVertices, const ChunkMeshVertex* vertices, ChunkRenderInfo* info);