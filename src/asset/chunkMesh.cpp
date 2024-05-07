#include "asset/chunkMesh.hpp"
#include "block.hpp"
#include "glHelpers/utils/logger/logger.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include <chrono>

#define NUM_CUBE_BLOCK_VERTICES 6
#define NUM_LAYER_INDEX_BITS 8
#define NUM_FACE_BITS 3
#define NUM_CHUNK_AXIS_BITS 6

static ChunkMeshVertex createChunkMeshVertex(GLuint layerIndex, BlockFace face, std::size_t x, std::size_t y, std::size_t z) {
    return 0 |
        (std::uint8_t)layerIndex |
        ((std::uint8_t)face << NUM_LAYER_INDEX_BITS) |
        ((std::uint8_t)x << (NUM_LAYER_INDEX_BITS + NUM_FACE_BITS)) |
        ((std::uint8_t)y << (NUM_LAYER_INDEX_BITS + NUM_FACE_BITS + 1*NUM_CHUNK_AXIS_BITS)) |
        ((std::uint8_t)z << (NUM_LAYER_INDEX_BITS + NUM_FACE_BITS + 2*NUM_CHUNK_AXIS_BITS));
}

static Logger logger("Chunk Mesh Builder", Logger::FGColors::BLUE);
static std::size_t numMeshesBuilt = 0;
static double totalMicroseconds = 0.0;

std::vector<ChunkMeshVertex> buildChunkMeshVertices(
    ChunkMeshBuildInfo info,
    const BlockTypeIdentArray* array,
    const BlockTypeIdentArray* frontArray,
    const BlockTypeIdentArray* backArray,
    const BlockTypeIdentArray* topArray,
    const BlockTypeIdentArray* bottomArray,
    const BlockTypeIdentArray* rightArray,
    const BlockTypeIdentArray* leftArray
) {
    auto clockStart = std::chrono::high_resolution_clock::now();

    std::vector<ChunkMeshVertex> vertices;

    // Chunk generation code
    for (std::size_t x = 0; x < NUM_CHUNK_AXIS_BLOCKS; x++) for (std::size_t y = 0; y < NUM_CHUNK_AXIS_BLOCKS; y++) for (std::size_t z = 0; z < NUM_CHUNK_AXIS_BLOCKS; z++) {
        // Get the block by using its local coordinates
        BlockTypeIdent ident = array->idents[x][y][z];

        if (ident == info.airIdent) { continue; }

        const BlockMeshBuildInfo* blockInfo = &info.blockMeshBuildInfos[ident];

        auto addFace = [&](BlockFace face) {
            GLuint layerIndex;
            switch (face) {
                case BlockFace::FRONT: layerIndex = blockInfo->faceLayerIndices.front; break;
                case BlockFace::BACK: layerIndex = blockInfo->faceLayerIndices.back; break;
                case BlockFace::TOP: layerIndex = blockInfo->faceLayerIndices.top; break;
                case BlockFace::BOTTOM: layerIndex = blockInfo->faceLayerIndices.bottom; break;
                case BlockFace::RIGHT: layerIndex = blockInfo->faceLayerIndices.right; break;
                case BlockFace::LEFT: layerIndex = blockInfo->faceLayerIndices.left; break;
            }

            ChunkMeshVertex vertex = createChunkMeshVertex(layerIndex, face, x, y, z);
            vertices.insert(vertices.end(), 6, vertex);
        };
        
        // +X Face Check
        if (x >= NUM_CHUNK_AXIS_BLOCKS - 1u) {
            if (!frontArray || frontArray->idents[0u][y][z] == info.airIdent) { addFace(BlockFace::FRONT); }
        } else if (array->idents[x + 1u][y][z] == info.airIdent) { addFace(BlockFace::FRONT); }
        
        // -X Face Check
        if (x < 1) {
            if (!backArray || backArray->idents[NUM_CHUNK_AXIS_BLOCKS - 1u][y][z] == info.airIdent) { addFace(BlockFace::BACK); }
        } else if (array->idents[x - 1u][y][z] == info.airIdent) { addFace(BlockFace::BACK); }
        
        // +Y Face Check
        if (y >= NUM_CHUNK_AXIS_BLOCKS - 1u) {
            if (!topArray || topArray->idents[x][0u][z] == info.airIdent) { addFace(BlockFace::TOP); }
        } else if (array->idents[x][y + 1u][z] == info.airIdent) { addFace(BlockFace::TOP); }
        
        // -Y Face Check
        if (y < 1) {
            if (!bottomArray || bottomArray->idents[x][NUM_CHUNK_AXIS_BLOCKS - 1u][z] == info.airIdent) { addFace(BlockFace::BOTTOM); }
        } else if (array->idents[x][y - 1u][z] == info.airIdent) { addFace(BlockFace::BOTTOM); }
        
        // +Z Face Check
        if (z >= NUM_CHUNK_AXIS_BLOCKS - 1u) {
            if (!rightArray || rightArray->idents[x][y][0u] == info.airIdent) { addFace(BlockFace::RIGHT); }
        } else if (array->idents[x][y][z + 1u] == info.airIdent) { addFace(BlockFace::RIGHT); }
        
        // -Z Face Check
        if (z < 1) {
            if (!leftArray || leftArray->idents[x][y][NUM_CHUNK_AXIS_BLOCKS - 1u] == info.airIdent) { addFace(BlockFace::LEFT); }
        } else if (array->idents[x][y][z - 1u] == info.airIdent) { addFace(BlockFace::LEFT); }
    }

    numMeshesBuilt++;

    auto clockEnd = std::chrono::high_resolution_clock::now();
    double microseconds = std::chrono::duration<double, std::micro>(clockEnd - clockStart).count();
    totalMicroseconds += microseconds;

    double avgMicroseconds = totalMicroseconds / ((double) numMeshesBuilt);

    logger.info("Chunk mesh built in " + std::to_string(microseconds) + "μs");
    logger.info("Average mesh build time: " + std::to_string(avgMicroseconds) + "μs");

    return vertices;
}