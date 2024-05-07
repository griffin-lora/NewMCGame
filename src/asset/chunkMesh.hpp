#pragma once
#include "block.hpp"
#include <GL/glew.h>
#include <vector>

using ChunkMeshVertex = GLuint;

struct BlockFaceLayerIndices {
    GLuint front;
    GLuint back;
    GLuint top;
    GLuint bottom;
    GLuint right;
    GLuint left;
};

struct BlockMeshBuildInfo {
    BlockFaceLayerIndices faceLayerIndices;
};

struct ChunkMeshBuildInfo {
    BlockTypeIdent airIdent;
    const BlockMeshBuildInfo* blockMeshBuildInfos;
};

std::vector<ChunkMeshVertex> buildChunkMeshVertices(
    ChunkMeshBuildInfo info,
    const BlockTypeIdentArray* array,
    const BlockTypeIdentArray* frontArray,
    const BlockTypeIdentArray* backArray,
    const BlockTypeIdentArray* topArray,
    const BlockTypeIdentArray* bottomArray,
    const BlockTypeIdentArray* rightArray,
    const BlockTypeIdentArray* leftArray
);