#include "ChunkMesh.hpp"

#include <GL/gl.h>
#include <GL/glew.h>

#include <stdexcept>
#include <exception>

ChunkMesh::ChunkMesh() {}
ChunkMesh::ChunkMesh(ChunkMesh&& ochunk) {
    vertexCount = ochunk.vertexCount;
    vertexBuffer = ochunk.vertexBuffer;
    buffersCreated = ochunk.buffersCreated;

    ochunk.vertexCount = 0;
    ochunk.vertexBuffer = 0;
    ochunk.buffersCreated = false;
}

void ChunkMesh::operator=(ChunkMesh&& ochunk) {
    vertexCount = ochunk.vertexCount;
    vertexBuffer = ochunk.vertexBuffer;
    buffersCreated = ochunk.buffersCreated;

    ochunk.vertexCount = 0;
    ochunk.vertexBuffer = 0;
    ochunk.buffersCreated = false;
}

ChunkMesh::~ChunkMesh() {
    if (buffersCreated) deleteBuffers();
}

void ChunkMesh::createBuffers() {
    if (buffersCreated) throw std::runtime_error("Buffers already created!");

    glGenBuffers(1, &vertexBuffer);

    buffersCreated = true;
}

void ChunkMesh::deleteBuffers() {
    if (!buffersCreated) throw std::runtime_error("Buffers don't exist!");

    glDeleteBuffers(1, &vertexBuffer);

    buffersCreated = false;
}

void ChunkMesh::bufferChunkData(std::span<BlockVertex> vertices) {
    if (!buffersCreated) createBuffers();

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BlockVertex), vertices.data(), GL_STATIC_DRAW);

    vertexCount = vertices.size();
}