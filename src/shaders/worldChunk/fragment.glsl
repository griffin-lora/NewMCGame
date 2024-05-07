#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragTexCoord;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2DArray blockTextureArray;

void main() {
	color = texture(blockTextureArray, fragTexCoord);
}