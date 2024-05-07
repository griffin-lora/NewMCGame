#include "chunkRender.hpp"

GLuint chunkShader;
GLuint chunkViewProjectionLocation;
GLuint chunkPositionLocation;
GLuint chunkBlockTextureArrayLocation;

void renderChunk(glm::vec3 pos, const ChunkRenderInfo* info) {
    glUniform3fv(chunkPositionLocation, 1, &pos[0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, info->vertexBuffer);
    glVertexAttribIPointer(
        0,
        1,
        GL_UNSIGNED_INT,
        0,
        (void*)0
    );

    glDrawArrays(GL_TRIANGLES, 0, info->numVertices);

    // totalVertexCount += n;

    glDisableVertexAttribArray(0);
}