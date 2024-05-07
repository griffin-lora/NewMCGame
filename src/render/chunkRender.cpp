#include "chunkRender.hpp"

void renderChunk(glm::vec3 pos, const ChunkRenderInfo* info) {
    // Set the position origin of the chunk we are about to draw
    // glUniform3fv(chunkCoordID, 1, &chunkCoord[0]);

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