#version 330 core

in vec3 fragTexCoord;
in float lightingFactor;

out vec4 color;

uniform sampler2DArray blockTextureArray;

void main() {
	color = lightingFactor * texture(blockTextureArray, fragTexCoord);
}