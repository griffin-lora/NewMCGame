#include "assetGenerate/chunkMesh.hpp"
#include "assetInit/chunkInit.hpp"
#include "assetUpload/chunkUpload.hpp"
#include "block.hpp"
#include "glHelpers/utils/logger/logger.hpp"
#include "render/chunkRender.hpp"
#include <cstddef>
#include <memory>
#include <vector>

int main(int argc, char** argv) {
    Logger logger("Main", Logger::FGColors::WHITE);

    std::unique_ptr<BlockTypeIdentArray> chunkBlockIdentArrays[4][4][4];

    for (std::size_t cx = 0; cx < 4; cx++) for (std::size_t cy = 0; cy < 4; cy++) for (std::size_t cz = 0; cz < 4; cz++) {
        // chunkBlockIdentArrays[cx][cy][cz] = std::make_unique<BlockTypeIdentArray>();
        BlockTypeIdentArray* array = chunkBlockIdentArrays[cx][cy][cz].get();

        for (std::size_t x = 0; x < NUM_CHUNK_AXIS_BLOCKS; x++) for (std::size_t y = 0; y < NUM_CHUNK_AXIS_BLOCKS; y++) for (std::size_t z = 0; z < NUM_CHUNK_AXIS_BLOCKS; z++) {
            if (y <= 4) {
                array->idents[x][y][z] = 1;
            } else {
                array->idents[x][y][z] = 0;
            }
        }
    }

    logger.info("Populated block type ident arrays");

    std::vector<ChunkMeshVertex> vertices = generateChunkMeshVertices(
        chunkBlockIdentArrays[0][0][0].get(),
        chunkBlockIdentArrays[1][0][0].get(),
        nullptr,
        chunkBlockIdentArrays[0][1][0].get(),
        nullptr,
        chunkBlockIdentArrays[0][0][1].get(),
        nullptr
    );

    logger.info("Generated chunk mesh vertices");
    
    ChunkRenderInfo chunkRenderInfos[4][4][4];
    for (std::size_t x = 0; x < 4; x++) for (std::size_t y = 0; y < 4; y++) for (std::size_t z = 0; z < 4; z++) {
        initChunkRenderInfo(&chunkRenderInfos[x][y][z]);
    }

    uploadChunkMesh(vertices.size(), vertices.data(), &chunkRenderInfos[0][0][0]);

    for (;;) {
        for (std::size_t x = 0; x < 4; x++) for (std::size_t y = 0; y < 4; y++) for (std::size_t z = 0; z < 4; z++) {
            glm::vec3 pos = { x * NUM_CHUNK_AXIS_BLOCKS, y * NUM_CHUNK_AXIS_BLOCKS, z * NUM_CHUNK_AXIS_BLOCKS };
            renderChunk(pos, &chunkRenderInfos[x][y][z]);
        }
    }

    return 0;
}