#pragma once

#include <utility>
#include <glm/glm.hpp>


class Block {
public:
    struct BlockIndex { uint32_t value; };
    enum class BlockFace {
        NORTH, // +X
        SOUTH, // -X
        EAST,  // +Z
        WEST,  // -Z
        TOP,   // +Y
        BOTTOM // -Y
    };
private:
    BlockIndex index;
public:
    inline Block(BlockIndex _index): index(_index) {}

    inline BlockIndex getIndex() const { return index; }
};