#pragma once
#include "block.hpp"
#include <GL/glew.h>
#include <vector>

using ChunkMeshVertex = GLuint;

std::vector<ChunkMeshVertex> buildChunkMeshVertices(
    const BlockTypeIdentArray* array,
    const BlockTypeIdentArray* frontArray,
    const BlockTypeIdentArray* backArray,
    const BlockTypeIdentArray* topArray,
    const BlockTypeIdentArray* bottomArray,
    const BlockTypeIdentArray* rightArray,
    const BlockTypeIdentArray* leftArray
);