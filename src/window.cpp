#include "window.hpp"
#include <stdexcept>

GLFWwindow* window;

void initWindow() {
    // Init GLFW
    if (!glfwInit()) throw std::runtime_error("Unable to initialize GLFW!");

    // Window stuff
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Create GLFW window
    window = glfwCreateWindow(1300, 768, "MC Game", NULL, NULL);

    if (window == NULL) {
        throw std::runtime_error("Unable to create GLFW window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(window); // Initialize GLEW

    // Initialize GLEW
    glewExperimental = true; // Needed in core profile
    if (glewInit() != GLEW_OK) throw std::runtime_error("Unable to initialize GLEW");

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
    });
}

void renderWindow() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool isWindowActive() {
    return glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0;
}