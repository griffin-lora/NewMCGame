#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern GLFWwindow* window;

void initWindow();
void renderWindow();
bool isWindowActive();