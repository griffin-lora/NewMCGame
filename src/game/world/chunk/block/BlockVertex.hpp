#pragma once

#include "world/chunk/block/block.hpp"
#include <GL/gl.h>
#include <cstdint>

#define NUM_LAYER_BITS 8
#define NUM_FACE_BITS 3
#define NUM_CHUNK_AXIS_BITS 6

class BlockVertex {
private:
    GLuint value;
public:
    // Ported from one of my earlier projects, I used to have some idea of how it worked back then but I don't anymore
    inline BlockVertex(uint8_t layer, Block::BlockFace face, uint8_t x, uint8_t y, uint8_t z): value(0 |
        layer |
        ((uint8_t)face << NUM_LAYER_BITS) |
        (x << (NUM_LAYER_BITS + NUM_FACE_BITS)) |
        (y << (NUM_LAYER_BITS + NUM_FACE_BITS + 1*NUM_CHUNK_AXIS_BITS)) |
        (z << (NUM_LAYER_BITS + NUM_FACE_BITS + 2*NUM_CHUNK_AXIS_BITS))
    ) {}
};