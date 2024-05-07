#version 330 core

#define GET_BITMASK(NUM_BITS) ((1u << (NUM_BITS)) - 1u)

#define NUM_LAYER_INDEX_BITS 8u
#define NUM_FACE_BITS 3u
#define NUM_CHUNK_AXIS_BITS 6u

uint layerIndexBitmask = GET_BITMASK(NUM_LAYER_INDEX_BITS);
uint faceBitmask = GET_BITMASK(NUM_FACE_BITS);
uint axisBitmask = GET_BITMASK(NUM_CHUNK_AXIS_BITS);

#define NUM_CUBE_BLOCK_VERTICES 36u
#define NUM_CUBE_BLOCK_FACE_VERTICES 6u

layout(location = 0) in uint vertexInfo;

out vec3 fragTexCoord;

uniform mat4 viewProjection;
uniform vec3 chunkPosition;

vec3[] cubePositions = vec3[NUM_CUBE_BLOCK_VERTICES](
    // Front +X
    vec3(1.000000, 1.000000, -0.000000),
    vec3(1.000000, 0.000000, -0.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(1.000000, 1.000000, -0.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(1.000000, 1.000000, -1.000000),
    // Back -X
    vec3(0.000000, 0.000000, -0.000000),
    vec3(-0.000000, 1.000000, -0.000000),
    vec3(-0.000000, 1.000000, -1.000000),
    vec3(0.000000, 0.000000, -0.000000),
    vec3(-0.000000, 1.000000, -1.000000),
    vec3(0.000000, 0.000000, -1.000000),
    // Top +Y
    vec3(0.000000, 1.000000, -0.000000),
    vec3(1.000000, 1.000000, -0.000000),
    vec3(1.000000, 1.000000, -1.000000),
    vec3(0.000000, 1.000000, -0.000000),
    vec3(1.000000, 1.000000, -1.000000),
    vec3(0.000000, 1.000000, -1.000000),
    // Bottom -Y
    vec3(0.000000, 0.000000, -1.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(1.000000, -0.000000, -0.000000),
    vec3(0.000000, 0.000000, -1.000000),
    vec3(1.000000, -0.000000, -0.000000),
    vec3(0.000000, -0.000000, -0.000000),
    // Right +Z
    vec3(0.000000, 0.000000, -0.000000),
    vec3(1.000000, 0.000000, -0.000000),
    vec3(1.000000, 1.000000, 0.000000),
    vec3(0.000000, 0.000000, -0.000000),
    vec3(1.000000, 1.000000, 0.000000),
    vec3(0.000000, 1.000000, 0.000000),
    // Left -Z
    vec3(0.000000, 1.000000, -1.000000),
    vec3(1.000000, 1.000000, -1.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(0.000000, 1.000000, -1.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(0.000000, 0.000000, -1.000000)
);

// Texcoord pain
vec2[] cubeTexCoords = vec2[NUM_CUBE_BLOCK_VERTICES](
    // Front +X
    vec2(1.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(-0.000000, 1.000000),
    vec2(1.000000, 0.000000),
    vec2(-0.000000, 1.000000),
    vec2(0.000000, 0.000000),
    // Back -X
    vec2(-0.000000, 1.000000),
    vec2(0.000000, 0.000000),
    vec2(1.000000, 0.000000),
    vec2(-0.000000, 1.000000),
    vec2(1.000000, 0.000000),
    vec2(1.000000, 1.000000),
    // Top +Y
    vec2(0.000000, 0.000000),
    vec2(1.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 1.000000),
    // Bottom -Y
    vec2(0.000000, 0.000000),
    vec2(1.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 1.000000),
    // Right +Z
    vec2(1.000000, 1.000000),
    vec2(0.000000, 1.000000),
    vec2(0.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 0.000000),
    vec2(1.000000, 0.000000),
    // Left -Z
    vec2(0.000000, 0.000000),
    vec2(1.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 0.000000),
    vec2(1.000000, 1.000000),
    vec2(0.000000, 1.000000)
);

void main() {
    uint layerIndex = vertexInfo & layerIndexBitmask;
    uint face = (vertexInfo >> NUM_LAYER_INDEX_BITS) & faceBitmask;

    uint x = (vertexInfo >> (NUM_LAYER_INDEX_BITS + NUM_FACE_BITS)) & axisBitmask;
    uint y = (vertexInfo >> (NUM_LAYER_INDEX_BITS + NUM_FACE_BITS + 1u*NUM_CHUNK_AXIS_BITS)) & axisBitmask;
    uint z = (vertexInfo >> (NUM_LAYER_INDEX_BITS + NUM_FACE_BITS + 2u*NUM_CHUNK_AXIS_BITS)) & axisBitmask;

    uint cubeVertexIndex = (face * NUM_CUBE_BLOCK_FACE_VERTICES) + (uint(gl_VertexID) % NUM_CUBE_BLOCK_FACE_VERTICES);
    vec3 vertexPosition = cubePositions[cubeVertexIndex];

    vec3 blockPosition = vec3(x, y, z);

	gl_Position = viewProjection * vec4(blockPosition + vertexPosition + chunkPosition, 1.0);
	fragTexCoord = vec3(cubeTexCoords[cubeVertexIndex], layerIndex);
}
