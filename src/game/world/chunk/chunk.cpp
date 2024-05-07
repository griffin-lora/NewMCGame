#include "chunk.hpp"

#include <cstring>
#include <memory>
#include <ratio>
#include <string.h>
#include "../world.hpp"
#include "world/chunk/block/BlockVertex.hpp"
#include <stdexcept>
#include <chrono>
#include <string>

Chunk::Chunk(): blockIndexArray(std::make_unique<BlockIndexArray>()) {}
Chunk::Chunk(Chunk&& ochunk) {
    blockIndexArray = std::move(ochunk.blockIndexArray);
    mesh = std::move(ochunk.mesh);
    meshUpdatedNeeded = ochunk.meshUpdatedNeeded;
}
Chunk::~Chunk() {}

const ChunkMesh& Chunk::getMeshRef() const {
    return mesh;
}

void Chunk::markRerender() {
    meshUpdatedNeeded = true;
}
bool Chunk::pendingMeshUpdate() const {
    return meshUpdatedNeeded;
}

Block Chunk::getBlock(glm::vec3 pos) {
    return Block(blockIndexArray->indices[(std::size_t) pos.y][(std::size_t) pos.z][(std::size_t) pos.x]);
}

void Chunk::setBlock(glm::vec3 pos, Block block) {
    blockIndexArray->indices[(std::size_t) pos.y][(std::size_t) pos.z][(std::size_t) pos.x] = block.getIndex();
    meshUpdatedNeeded = true;
}

static Logger logger("Chunk Mesh Builder", Logger::FGColors::BLUE);
static size_t numMeshesBuilt = 0;
static double totalMicroseconds = 0.0;

void Chunk::buildMesh(World& world, glm::vec3 chunkCoords) {
    auto clockStart = std::chrono::high_resolution_clock::now();

    meshUpdatedNeeded = false;

    std::vector<BlockVertex> vertices;

    const BlockIndexArray* indices = blockIndexArray.get();

    const BlockIndexArray* northChunkIndices = chunkCoords.x + 1 >= world.chunkSizeX() ? nullptr : world.getChunkRef(chunkCoords + glm::vec3(1, 0, 0)).blockIndexArray.get();
    const BlockIndexArray* southChunkIndices = chunkCoords.x - 1 < 0 ? nullptr : world.getChunkRef(chunkCoords + glm::vec3(-1, 0, 0)).blockIndexArray.get();
    const BlockIndexArray* eastChunkIndices = chunkCoords.z + 1 >= world.chunkSizeZ() ? nullptr : world.getChunkRef(chunkCoords + glm::vec3(0, 0, 1)).blockIndexArray.get();
    const BlockIndexArray* westChunkIndices = chunkCoords.z - 1 < 0 ? nullptr : world.getChunkRef(chunkCoords + glm::vec3(0, 0, -1)).blockIndexArray.get();
    const BlockIndexArray* topChunkIndices = chunkCoords.y + 1 >= world.chunkSizeY() ? nullptr : world.getChunkRef(chunkCoords + glm::vec3(0, 1, 0)).blockIndexArray.get();
    const BlockIndexArray* bottomChunkIndices = chunkCoords.y - 1 < 0 ? nullptr : world.getChunkRef(chunkCoords + glm::vec3(0, -1, 0)).blockIndexArray.get();

    auto& blockDB = world.getBlockDBRef();

    // Chunk generation code
    for (size_t y=0; y<16; y++) {
        for (size_t z=0; z<16; z++) {
            for (size_t x=0; x<16; x++) {
                // Get the block by using its chunk coordinates
                Block::BlockIndex blockIndex = indices->indices[y][z][x];

                if (blockDB.isAir(blockIndex)) continue;

                const BlockDB::BlockMeshInfo& meshInfo = blockDB.getBlockMeshInfo(blockIndex);

                using namespace glm;

                auto addFace = [&](Block::BlockFace face) {
                    int layerId;
                    switch(face){
                        case Block::BlockFace::NORTH:  layerId = meshInfo.faces.north; break;
                        case Block::BlockFace::SOUTH:  layerId = meshInfo.faces.south; break;
                        case Block::BlockFace::EAST:   layerId = meshInfo.faces.east;  break;
                        case Block::BlockFace::WEST:   layerId = meshInfo.faces.west;  break;
                        case Block::BlockFace::TOP:    layerId = meshInfo.faces.top;   break;
                        case Block::BlockFace::BOTTOM: layerId = meshInfo.faces.bottom; break;
                    }

                    BlockVertex vertex(layerId, face, x, y, z);
                    vertices.insert(vertices.end(), 6, vertex);
                };
                
                // +X Face Check
                if (x >= 15) {
                    if (!northChunkIndices || blockDB.isAir(northChunkIndices->indices[y][z][0u]))
                        addFace(Block::BlockFace::NORTH);
                } else if (blockDB.isAir(indices->indices[y][z][x + 1u]))
                    addFace(Block::BlockFace::NORTH);

                // -X Face Check
                if (x < 1) {
                    if (!southChunkIndices || blockDB.isAir(southChunkIndices->indices[y][z][15u]))
                        addFace(Block::BlockFace::SOUTH);
                } else if (blockDB.isAir(indices->indices[y][z][x - 1u]))
                    addFace(Block::BlockFace::SOUTH);

                // +Z Face Check
                if (z >= 15) {
                    if (!eastChunkIndices || blockDB.isAir(eastChunkIndices->indices[y][0u][x]))
                        addFace(Block::BlockFace::EAST);
                } else if (blockDB.isAir(indices->indices[y][z + 1u][x]))
                    addFace(Block::BlockFace::EAST);

                // -Z Face Check
                if (z < 1) {
                    if (!westChunkIndices || blockDB.isAir(westChunkIndices->indices[y][15u][x]))
                        addFace(Block::BlockFace::WEST);
                } else if (blockDB.isAir(indices->indices[y][z - 1u][x]))
                    addFace(Block::BlockFace::WEST);

                // +Y Face Check
                if (y >= 15) {
                    if (!topChunkIndices || blockDB.isAir(topChunkIndices->indices[0u][z][x]))
                        addFace(Block::BlockFace::TOP);
                } else if (blockDB.isAir(indices->indices[y + 1u][z][x]))
                    addFace(Block::BlockFace::TOP);

                // -Y Face Check
                if (y < 1) {
                    if (!bottomChunkIndices || blockDB.isAir(bottomChunkIndices->indices[15u][z][x]))
                        addFace(Block::BlockFace::BOTTOM);
                } else if (blockDB.isAir(indices->indices[y - 1u][z][x]))
                    addFace(Block::BlockFace::BOTTOM);
            }
        }
    }

    numMeshesBuilt++;

    auto clockEnd = std::chrono::high_resolution_clock::now();
    double microseconds = std::chrono::duration<double, std::micro>(clockEnd - clockStart).count();
    totalMicroseconds += microseconds;

    double avgMicroseconds = totalMicroseconds / ((double) numMeshesBuilt);

    logger.info("Chunk { " + std::to_string(chunkCoords.x) + ", " + std::to_string(chunkCoords.y) + ", " + std::to_string(chunkCoords.z) + " } mesh built in " + std::to_string(microseconds) + "μs");
    logger.info("Average mesh build time: " + std::to_string(avgMicroseconds) + "μs");
    
    mesh.bufferChunkData(vertices);
}

std::vector<std::uint8_t> Chunk::serialize() const {
    const std::uint8_t* blockIndicesData = (std::uint8_t*)blockIndexArray->indices;
    return std::vector<std::uint8_t>(blockIndicesData, blockIndicesData + sizeof(blockIndexArray->indices));
}
void Chunk::deserialize(const std::vector<std::uint8_t>& data) {
    const Block::BlockIndex* blockIndicesData = (Block::BlockIndex*)data.data();
    std::memcpy(blockIndexArray->indices, data.data(), sizeof(blockIndexArray->indices));
}

std::size_t Chunk::serializedChunkSize() {
    return 16 * 16 * 16 * sizeof(Block);
}