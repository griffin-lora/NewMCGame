#pragma once

#include <cstdint>

#define NUM_CHUNK_AXIS_BLOCKS 16u

using BlockTypeIdent = std::uint32_t;

struct BlockTypeIdentArray {
    BlockTypeIdent idents[NUM_CHUNK_AXIS_BLOCKS][NUM_CHUNK_AXIS_BLOCKS][NUM_CHUNK_AXIS_BLOCKS];
};

enum class BlockFace {
    FRONT,
    BACK,
    TOP,
    BOTTOM,
    RIGHT,
    LEFT
};