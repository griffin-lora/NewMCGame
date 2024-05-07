#pragma once

#include "assetGenerate/chunkMesh.hpp"
#include "render/chunkRender.hpp"

void uploadChunkMesh(std::size_t numVertices, const ChunkMeshVertex* vertices, ChunkRenderInfo* info);