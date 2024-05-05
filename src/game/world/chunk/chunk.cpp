#include "chunk.hpp"

#include <ratio>
#include <string.h>
#include "../world.hpp"
#include "world/chunk/block/BlockVertex.hpp"
#include <stdexcept>
#include <chrono>
#include <string>

Block::BlockIndex& Chunk::getBlockIndex(glm::vec3 localPos) {
    int i = (int)localPos.x + (int)localPos.z * 16 + (int)localPos.y * 16 * 16;
    return blockIndices[i];
}

Block::BlockIndex& Chunk::getBlockIndexAt(glm::vec3 localPos) {
    int i = (int)localPos.x + (int)localPos.z * 16 + (int)localPos.y * 16 * 16;
    return blockIndices.at(i);
}

Chunk::Chunk() {
    blockIndices.resize(16*16*16);
}
Chunk::Chunk(Chunk&& ochunk) {
    blockIndices = std::move(ochunk.blockIndices);
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
    return Block(getBlockIndexAt(pos));
}

void Chunk::setBlock(glm::vec3 pos, Block block) {
    getBlockIndexAt(pos) = block.getIndex();
    meshUpdatedNeeded = true;
}

static Logger logger("Chunk Mesh Builder", Logger::FGColors::BLUE);
static size_t numMeshesBuilt = 0;
static double totalMicroseconds = 0.0;

void Chunk::buildMesh(World& world, glm::vec3 chunkCoords) {

    auto clockStart = std::chrono::high_resolution_clock::now();

    meshUpdatedNeeded = false;

    std::vector<BlockVertex> vertices;

    Chunk* northChunk = chunkCoords.x + 1 >= world.chunkSizeX() ? nullptr : &world.getChunkRef(chunkCoords + glm::vec3(1, 0, 0));
    Chunk* southChunk = chunkCoords.x - 1 < 0 ? nullptr : &world.getChunkRef(chunkCoords + glm::vec3(-1, 0, 0));
    Chunk* eastChunk = chunkCoords.z + 1 >= world.chunkSizeZ() ? nullptr : &world.getChunkRef(chunkCoords + glm::vec3(0, 0, 1));
    Chunk* westChunk = chunkCoords.z - 1 < 0 ? nullptr : &world.getChunkRef(chunkCoords + glm::vec3(0, 0, -1));
    Chunk* topChunk = chunkCoords.y + 1 >= world.chunkSizeY() ? nullptr : &world.getChunkRef(chunkCoords + glm::vec3(0, 1, 0));
    Chunk* bottomChunk = chunkCoords.y - 1 < 0 ? nullptr : &world.getChunkRef(chunkCoords + glm::vec3(0, -1, 0));

    auto& blockDB = world.getBlockDBRef();

    // Chunk generation code
    for (int y=0; y<16; y++) {
        for (int z=0; z<16; z++) {
            for (int x=0; x<16; x++) {
                auto cPos = glm::vec3(x, y, z);

                // Get the block by using its chunk coordinates
                auto blockIndex = getBlockIndex(cPos);

                if (blockDB.isAir(blockIndex)) continue;

                const BlockDB::BlockMeshInfo& meshInfo = blockDB.getBlockMeshInfo(blockIndex);

                using namespace glm;

                auto addFace = [&](std::array<glm::vec3, 6> verts, Block::BlockFace face) {
                    int layerId;
                    switch(face){
                        case Block::BlockFace::NORTH:  layerId = meshInfo.faces.north; break;
                        case Block::BlockFace::SOUTH:  layerId = meshInfo.faces.south; break;
                        case Block::BlockFace::EAST:   layerId = meshInfo.faces.east;  break;
                        case Block::BlockFace::WEST:   layerId = meshInfo.faces.west;  break;
                        case Block::BlockFace::TOP:    layerId = meshInfo.faces.top;   break;
                        case Block::BlockFace::BOTTOM: layerId = meshInfo.faces.bottom; break;
                    }

                    for (int i = 0; i < 6; i++) {
                        vertices.push_back(BlockVertex(layerId, face, cPos.x, cPos.y, cPos.z));
                    }
                };
                
                // +X Face Check
                const std::array<glm::vec3, 6> xPos = {
                    glm::vec3(x + 1, y, z), glm::vec3(x + 1, y + 1, z),     glm::vec3(x + 1, y + 1, z + 1),
                    glm::vec3(x + 1, y, z), glm::vec3(x + 1, y + 1, z + 1), glm::vec3(x + 1, y, z + 1),
                };
                if (x >= 15) {
                    if (!northChunk || blockDB.isAir(northChunk->getBlockIndex(vec3(0.f, cPos.y, cPos.z))))
                        addFace(xPos, Block::BlockFace::NORTH);
                } else if (blockDB.isAir(getBlockIndex(cPos + vec3(1, 0, 0))))
                    addFace(xPos, Block::BlockFace::NORTH);

                // -X Face Check
                const std::array<glm::vec3, 6> xNeg = {
                    glm::vec3(x, y, z + 1), glm::vec3(x, y + 1, z + 1), glm::vec3(x, y + 1, z),
                    glm::vec3(x, y, z + 1), glm::vec3(x, y + 1, z),     glm::vec3(x, y, z),
                };
                if (x < 1) {
                    if (!southChunk || blockDB.isAir(southChunk->getBlockIndex(vec3(15.f, cPos.y, cPos.z))))
                        addFace(xNeg, Block::BlockFace::SOUTH);
                } else if (blockDB.isAir(getBlockIndex(cPos + vec3(-1, 0, 0))))
                    addFace(xNeg, Block::BlockFace::SOUTH);

                // +Z Face Check
                const std::array<glm::vec3, 6> zPos = {
                    glm::vec3(x + 1, y, z + 1), glm::vec3(x + 1, y + 1, z + 1), glm::vec3(x, y + 1, z + 1),
                    glm::vec3(x + 1, y, z + 1), glm::vec3(x, y + 1, z + 1),     glm::vec3(x, y, z + 1),
                };
                if (z >= 15) {
                    if (!eastChunk || blockDB.isAir(eastChunk->getBlockIndex(vec3(cPos.x, cPos.y, 0))))
                        addFace(zPos, Block::BlockFace::EAST);
                } else if (blockDB.isAir(getBlockIndex(cPos + vec3(0, 0, 1))))
                    addFace(zPos, Block::BlockFace::EAST);

                // -Z Face Check
                const std::array<glm::vec3, 6> zNeg = {
                    glm::vec3(x, y, z), glm::vec3(x, y + 1, z),     glm::vec3(x + 1, y + 1, z),
                    glm::vec3(x, y, z), glm::vec3(x + 1, y + 1, z), glm::vec3(x + 1, y, z),
                };
                if (z < 1) {
                    if (!westChunk || blockDB.isAir(westChunk->getBlockIndex(vec3(cPos.x, cPos.y, 15))))
                        addFace(zNeg, Block::BlockFace::WEST);
                } else if (blockDB.isAir(getBlockIndex(cPos + vec3(0, 0, -1))))
                    addFace(zNeg, Block::BlockFace::WEST);

                // +Y Face Check
                const std::array<glm::vec3, 6> yPos = {
                    glm::vec3(x, y + 1, z), glm::vec3(x, y + 1, z + 1),     glm::vec3(x + 1, y + 1, z + 1),
                    glm::vec3(x, y + 1, z), glm::vec3(x + 1, y + 1, z + 1), glm::vec3(x + 1, y + 1, z),
                };
                if (y >= 15) {
                    if (!topChunk || blockDB.isAir(topChunk->getBlockIndex(vec3(cPos.x, 0, cPos.z))))
                        addFace(yPos, Block::BlockFace::TOP);
                } else if (blockDB.isAir(getBlockIndex(cPos + vec3(0, 1, 0))))
                    addFace(yPos, Block::BlockFace::TOP);

                // -Y Face Check
                const std::array<glm::vec3, 6> yNeg = {
                    glm::vec3(x, y, z), glm::vec3(x + 1, y, z),     glm::vec3(x + 1, y, z + 1),
                    glm::vec3(x, y, z), glm::vec3(x + 1, y, z + 1), glm::vec3(x, y, z + 1),
                };
                if (y < 1) {
                    if (!bottomChunk || blockDB.isAir(bottomChunk->getBlockIndex(vec3(cPos.x, 15, cPos.z))))
                        addFace(yNeg, Block::BlockFace::BOTTOM);
                } else if (blockDB.isAir(getBlockIndex(cPos + vec3(0, -1, 0))))
                    addFace(yNeg, Block::BlockFace::BOTTOM);
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
    const std::uint8_t* blockIndicesData = (std::uint8_t*)blockIndices.data();
    return std::vector<std::uint8_t>(blockIndicesData, blockIndicesData + blockIndices.size() * sizeof(Block::BlockIndex));
}
void Chunk::deserialize(const std::vector<std::uint8_t>& data) {
    const Block::BlockIndex* blockIndicesData = (Block::BlockIndex*)data.data();
    blockIndices = std::vector<Block::BlockIndex>(blockIndicesData, blockIndicesData + data.size());
}

std::size_t Chunk::serializedChunkSize() {
    return 16 * 16 * 16 * sizeof(Block);
}