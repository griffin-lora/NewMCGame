#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <span>

#include "utils/NotCopyable.hpp"
#include "world/chunk/block/BlockVertex.hpp"

class ChunkMesh : public NonCopyable {
private:
    std::size_t vertexCount = 0;
    GLuint vertexBuffer;
    bool buffersCreated = false;
public:
    ChunkMesh();
    ChunkMesh(ChunkMesh&&);
    void operator=(ChunkMesh&&);
    ~ChunkMesh();

    void createBuffers();
    void deleteBuffers();
    void bufferChunkData(std::span<BlockVertex> vertices);

    inline std::size_t getVertexCount() const { return vertexCount; }
    inline bool buffersReady() const { return buffersCreated; }
    inline GLuint getVertexBuffer() const { return vertexBuffer; }
};