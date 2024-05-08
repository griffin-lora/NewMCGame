#pragma once

#include <GL/glew.h>
#include <string>

struct ShaderSource {
    std::string vertexSource;
    std::string fragmentSource;
};

/**
 * Loads in shaders
 * @param shaderResourcePath Path to the directory with the vertex and fragment shader
*/
ShaderSource loadShaders(const std::string& shaderResourcePath);

/**
 * Creates a program for shaders
 * @param vertexSource Vertex shader source
 * @param fragmentSource Fragment shader source
*/
GLuint initShaders(const ShaderSource& source);