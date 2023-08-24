#pragma once

#include <map>
#include <tuple>
#include <vector>
#include <string>

#include "chunk/chunk.hpp"

class World {
public:
    using ChunkPos = std::tuple<int, int, int>;

    struct SerializedWorldData {
        int chunkSizeX, chunkSizeY, chunkSizeZ;
    };
private:
    std::map<ChunkPos, Chunk> chunks;
    int sizeX;
    int sizeY;
    int sizeZ;

    Chunk& getChunkFromWorldCoords(glm::vec3 pos);
    
    bool gfxReady = false;
    GLuint worldShader;
    GLuint viewProjectionID, chunkCoordID, textureAtlasID;
    GLuint textureAtlas;
public:
    World();
    ~World();

    // Does all the necessary calls to OpenGL related functions
    void gfxInit();
    
    int chunkSizeX() const;
    int chunkSizeY() const;
    int chunkSizeZ() const;

    Block getBlock(glm::vec3 pos);
    void setBlock(glm::vec3 pos, Block block, bool noCheck = false);

    // Returns true if the coordinates are in the world
    bool coordinatesInWorld(glm::vec3 pos);

    // This could take awhile
    void generateWorld(int xs, int ys, int zs);

    Chunk& getChunk(glm::vec3 chunkCoords);

    std::map<ChunkPos, Chunk>& getChunks();

    void update();

    void draw(const glm::mat4& viewProjection);

    std::size_t getNVertices() const;

    void saveWorld(std::string saveName) const;
    void loadWorld(std::string saveName);

    bool worldSaveExists(std::string saveName) const;

    inline static glm::vec3 chunkPosToVec3(ChunkPos pos) {
        return glm::vec3(std::get<0>(pos), std::get<1>(pos), std::get<2>(pos));
    }
};
