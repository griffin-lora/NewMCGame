#include "asset/chunkMesh.hpp"
#include "block.hpp"
#include "glHelpers/utils/logger/logger.hpp"
#include <cstddef>
#include <vector>
#include <chrono>

#define NUM_CUBE_BLOCK_VERTICES 6
#define NUM_LAYER_INDEX_BITS 8
#define NUM_FACE_BITS 3
#define NUM_CHUNK_AXIS_BITS 6

static ChunkMeshVertex createChunkMeshVertex(GLuint layer_index, BlockFace face, std::size_t x, std::size_t y, std::size_t z) {
    return 0 |
        (std::uint8_t)layer_index |
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
        
        // // +X Face Check (FRONT)
        // if (x >= 15) {
        //     if (!frontArray || frontArray->idents[0u][y][z] == info.airIdent) {
        //         vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(
        //             blockInfo->faceLayerIndices.front, BlockFace::FRONT, x, y, z
        //         ));
        //     }
        // } else if (frontArray->idents[x + 1u][y][z] == info.airIdent) {
        //     vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(
        //         blockInfo->faceLayerIndices.front, BlockFace::FRONT, x, y, z
        //     ));
        // }

        // +X Face Check (FRONT)
        if ((x >= 15 && (!frontArray || frontArray->idents[0u][y][z] == info.airIdent)) || array->idents[x + 1u][y][z] == info.airIdent) {
            vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(blockInfo->faceLayerIndices.front, BlockFace::FRONT, x, y, z));
        }

        // -X Face Check (BACK)
        if ((x < 0 && (!backArray || backArray->idents[NUM_CHUNK_AXIS_BLOCKS - 1u][y][z] == info.airIdent)) || array->idents[x - 1u][y][z] == info.airIdent) {
            vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(blockInfo->faceLayerIndices.back, BlockFace::BACK, x, y, z));
        }

        // +Y Face Check (TOP)
        if ((y >= 15 && (!topArray || topArray->idents[x][0u][z] == info.airIdent)) || array->idents[x][y + 1u][z] == info.airIdent) {
            vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(blockInfo->faceLayerIndices.top, BlockFace::TOP, x, y, z));
        }

        // -Y Face Check (BOTTOM)
        if ((y < 0 && (!bottomArray || bottomArray->idents[x][NUM_CHUNK_AXIS_BLOCKS - 1u][z] == info.airIdent)) || array->idents[x][y - 1u][z] == info.airIdent) {
            vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(blockInfo->faceLayerIndices.bottom, BlockFace::BOTTOM, x, y, z));
        }

        // +Z Face Check (RIGHT)
        if ((z >= 15 && (!rightArray || rightArray->idents[x][y][0u] == info.airIdent)) || array->idents[x][y][z + 1u] == info.airIdent) {
            vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(blockInfo->faceLayerIndices.right, BlockFace::RIGHT, x, y, z));
        }

        // -X Face Check (LEFT)
        if ((x < 0 && (!leftArray || leftArray->idents[x][y][NUM_CHUNK_AXIS_BLOCKS - 1u] == info.airIdent)) || array->idents[x][y][z - 1u] == info.airIdent) {
            vertices.insert(vertices.end(), NUM_CUBE_BLOCK_VERTICES, createChunkMeshVertex(blockInfo->faceLayerIndices.left, BlockFace::LEFT, x, y, z));
        }
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