#include "asset/chunkMesh.hpp"
#include <vector>

std::vector<ChunkMeshVertex> buildChunkMeshVertices(
    const BlockTypeIdentArray* array,
    const BlockTypeIdentArray* frontArray,
    const BlockTypeIdentArray* backArray,
    const BlockTypeIdentArray* topArray,
    const BlockTypeIdentArray* bottomArray,
    const BlockTypeIdentArray* rightArray,
    const BlockTypeIdentArray* leftArray
) {
    return std::vector<ChunkMeshVertex>();
}