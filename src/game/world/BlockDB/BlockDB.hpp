#pragma once

#include <GL/gl.h>

#include <cstddef>
#include <map>
#include <vector>

#include "glHelpers/utils/logger/logger.hpp"
#include "world/chunk/block/block.hpp"

#define QuickNumMapImpl
#include "utils/QuickNumMap/QuickNumMap.hpp"

class BlockDB {
public:
    struct FaceLayers {
        GLuint top, bottom, north, south, east, west;
    };

    // struct BlockInfo {
    //     FaceLayers faces;
    //     std::uint64_t packHash;
    //     int blockId;
    // };

    struct BlockMeshInfo {
        FaceLayers faces;
    };
private:
    GLuint textureArrayId;
    bool texturesLoaded = false;
    bool gfxReady = false;

    Logger logger{ "BlockDB", Logger::FGColors::BLUE };

    std::map<std::string, Block::BlockIndex> blockNamesToIndices;
    std::vector<std::string> blockNames;
    std::vector<BlockMeshInfo> blockMeshInfos;
public:
    BlockDB();
    ~BlockDB();

    /**
     * Initiate any OpenGL components
    */
    void gfxInit();

    /**
     * Load all the files from BLOCKPACKS_FOLDER and load the texture defined in those files
    */
    void loadBlocks();

    /**
     * Get the ID of the created texture array
    */
    GLuint getTextureId();

    /**
     * Returns the block index for a block by name
     * @param name Name of the block
    */
    inline Block::BlockIndex getIndexByName(std::string name) const {
        return blockNamesToIndices.at(name);
    }

    /**
     * Get the BlockMeshInfo struct for a block of a certain index
     * @param index Block index to look up
    */
    inline const BlockMeshInfo& getBlockMeshInfo(Block::BlockIndex index) const {
        return blockMeshInfos[index.value];
    }

    /**
     * Checks if the block index given is air
     * @param index Block index to check against
    */
    inline bool isAir(Block::BlockIndex index) {
        return index.value == 0;
    }
};

#undef QuickNumMapImpl