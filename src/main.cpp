#include "asset/assetsLoad.hpp"
#include "asset/chunkMesh.hpp"
#include "assetGL/chunkAsset.hpp"
#include "assetGL/assetsInit.hpp"
#include "block.hpp"
#include "glHelpers/utils/logger/logger.hpp"
#include "glHelpers/utils/dotenv.h"
#include "player.hpp"
#include "render/chunkRender.hpp"
#include "window.hpp"
#include <cstddef>
#include <memory>
#include <vector>

int main(int argc, char** argv) {
    Logger logger("Main", Logger::FGColors::WHITE);

    dotenv::init();
    initWindow();

    {
        auto info = loadRenderAssets();
        if (!info.has_value()) {
            return 1;
        }
        initRenderAssets(info.value());
    }

    logger.info("Finished initialization");

    std::unique_ptr<BlockTypeIdentArray> chunkBlockIdentArrays[4][4][4];

    for (std::size_t cx = 0; cx < 4; cx++) for (std::size_t cy = 0; cy < 4; cy++) for (std::size_t cz = 0; cz < 4; cz++) {
        chunkBlockIdentArrays[cx][cy][cz] = std::make_unique<BlockTypeIdentArray>();
        BlockTypeIdentArray* array = chunkBlockIdentArrays[cx][cy][cz].get();

        for (std::size_t x = 0; x < NUM_CHUNK_AXIS_BLOCKS; x++) for (std::size_t y = 0; y < NUM_CHUNK_AXIS_BLOCKS; y++) for (std::size_t z = 0; z < NUM_CHUNK_AXIS_BLOCKS; z++) {
            if (y == 5) {
                array->idents[x][y][z] = 2;
            } else if (y <= 4) {
                array->idents[x][y][z] = 1;
            } else {
                array->idents[x][y][z] = 0;
            }
        }
    }

    logger.info("Populated block type ident arrays");

    BlockMeshBuildInfo blockMeshBuildInfos[] = {
        {},
        { .faceLayerIndices = {
            .front = 2,
            .back = 2,
            .top = 2,
            .bottom = 2,
            .right = 2,
            .left = 2
        } },
        { .faceLayerIndices = {
            .front = 0,
            .back = 0,
            .top = 1,
            .bottom = 2,
            .right = 0,
            .left = 0
        } }
    };
    
    ChunkRenderInfo chunkRenderInfos[4][4][4];


    for (std::size_t x = 0; x < 4; x++) for (std::size_t y = 0; y < 4; y++) for (std::size_t z = 0; z < 4; z++) {
        std::vector<ChunkMeshVertex> chunkMeshVertices = buildChunkMeshVertices(
            (ChunkMeshBuildInfo) {
                .airIdent = 0,
                .blockMeshBuildInfos = blockMeshBuildInfos
            },
            chunkBlockIdentArrays[x][y][z].get(),
            x >= 3u ? nullptr : chunkBlockIdentArrays[x + 1u][y][z].get(),
            x <= 0 ? nullptr : chunkBlockIdentArrays[x - 1u][y][z].get(),
            y >= 3u ? nullptr : chunkBlockIdentArrays[x][y + 1u][z].get(),
            y <= 0 ? nullptr : chunkBlockIdentArrays[x][y - 1u][z].get(),
            z >= 3u ? nullptr : chunkBlockIdentArrays[x][y][z + 1u].get(),
            z <= 0 ? nullptr : chunkBlockIdentArrays[x][y][z - 1u].get()
        );

        initChunkRenderInfo(&chunkRenderInfos[x][y][z]);
        uploadChunkMesh(chunkMeshVertices.size(), chunkMeshVertices.data(), &chunkRenderInfos[x][y][z]);
    }

    logger.info("Chunks ready to render");

    Player player(glm::vec3(1, 100, 1), glm::vec3(1, 0, 0), 70.f);

    do {
        player.updatePlayer(16.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(chunkShader);
        glUniformMatrix4fv(chunkViewProjectionLocation, 1, GL_FALSE, &player.getViewProjection()[0][0]);
        
        for (std::size_t x = 0; x < 4; x++) for (std::size_t y = 0; y < 4; y++) for (std::size_t z = 0; z < 4; z++) {
            glm::vec3 pos = { x * NUM_CHUNK_AXIS_BLOCKS, y * NUM_CHUNK_AXIS_BLOCKS, z * NUM_CHUNK_AXIS_BLOCKS };
            renderChunk(pos, &chunkRenderInfos[x][y][z]);
        }

        renderWindow();
    } while (isWindowActive());

    return 0;
}