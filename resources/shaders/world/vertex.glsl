#version 330 core

#define GET_BITMASK(NUM_BITS) ((1u << (NUM_BITS)) - 1u)

#define NUM_LAYER_BITS 8u
#define NUM_FACE_BITS 3u
#define NUM_CHUNK_AXIS_BITS 6u

uint layerBitmask = GET_BITMASK(NUM_LAYER_BITS);
uint faceBitmask = GET_BITMASK(NUM_FACE_BITS);
uint axisBitmask = GET_BITMASK(NUM_CHUNK_AXIS_BITS);

#define NUM_CUBE_BLOCK_VERTICES 36u
#define NUM_CUBE_BLOCK_FACE_VERTICES 6u

layout(location = 0) in uint vertexInfo;

out vec3 fragTexCoord;

uniform mat4 viewProjection;
uniform vec3 chunkCoord;

vec3[] cubePositions = vec3[NUM_CUBE_BLOCK_VERTICES](
    // North +X
    vec3(1.000000, 1.000000, -0.000000),
    vec3(1.000000, 0.000000, -0.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(1.000000, 1.000000, -0.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(1.000000, 1.000000, -1.000000),
    // South -X
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
    // East +Z
    vec3(0.000000, 0.000000, -0.000000),
    vec3(1.000000, 0.000000, -0.000000),
    vec3(1.000000, 1.000000, 0.000000),
    vec3(0.000000, 0.000000, -0.000000),
    vec3(1.000000, 1.000000, 0.000000),
    vec3(0.000000, 1.000000, 0.000000),
    // West -Z
    vec3(0.000000, 1.000000, -1.000000),
    vec3(1.000000, 1.000000, -1.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(0.000000, 1.000000, -1.000000),
    vec3(1.000000, 0.000000, -1.000000),
    vec3(0.000000, 0.000000, -1.000000)
);

// Keep in mind theese are for a texture that is upside down because stbi is wierd
vec2[] cubeTexCoords = vec2[6](
    vec2(0, 1), vec2(0, 0), vec2(1, 0),
    vec2(0, 1), vec2(1, 0), vec2(1, 1)
);

void main() {
    uint layer = vertexInfo & layerBitmask;
    uint face = (vertexInfo >> NUM_LAYER_BITS) & faceBitmask;

    uint x = (vertexInfo >> (NUM_LAYER_BITS + NUM_FACE_BITS)) & axisBitmask;
    uint y = (vertexInfo >> (NUM_LAYER_BITS + NUM_FACE_BITS + 1u*NUM_CHUNK_AXIS_BITS)) & axisBitmask;
    uint z = (vertexInfo >> (NUM_LAYER_BITS + NUM_FACE_BITS + 2u*NUM_CHUNK_AXIS_BITS)) & axisBitmask;

    uint cubeVertexIndex = (face * NUM_CUBE_BLOCK_FACE_VERTICES) + (uint(gl_VertexID) % NUM_CUBE_BLOCK_FACE_VERTICES);
    vec3 vertexPosition = cubePositions[cubeVertexIndex];

    vec3 blockPosition = vec3(x, y, z);

	gl_Position = viewProjection * vec4(blockPosition + vertexPosition + (chunkCoord * 16), 1.0);

	fragTexCoord = vec3(cubeTexCoords[gl_VertexID % int(NUM_CUBE_BLOCK_FACE_VERTICES)], layer);
}
