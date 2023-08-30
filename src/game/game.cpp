#include "game.hpp"

#include <exception>
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader/shader.hpp"
#include "texture/texture.hpp"
#include "world/debug/ChunkBorderDebugger.hpp"
#include "utils/path.hpp"

template <typename T, typename... Args>
using FP = T(*)(Args...);

Game::Game():
    player{glm::vec3(0, 50, 0), glm::vec3(1, 0, 0), 70.f}
{
    std::cout << "WARNING: Make sure the CWD is the base path of the folder!!! Or else nothing will load correctly\n";

    // Initiate GLFW / glew / OpenGL
    gfxInit();

    // VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Initializes all the shaders/textures/buffers
    world.gfxInit();
    billboardManager.gfxInit();
    hudTextManager.gfxInit();

    // Generate world
    if (world.worldSaveExists("test")) {
        world.loadWorld("test");
    } else {
        world.generateWorld(10, 5, 10);
    }
    
    // Load the plugins and enable them
    pluginManager.loadPlugins();
    pluginManager.enablePlugins(*this);

    // Resize the viewport on window size change
    glfwSetFramebufferSizeCallback(gameWindow, [](GLFWwindow* win, int w, int h) {
        glViewport(0, 0, w, h);
    });
}
Game::~Game() {
    pluginManager.disablePlugins(*this);
    pluginManager.unloadPlugins();

    glDeleteVertexArrays(1, &vertexArrayID);
    glfwTerminate();
}
void Game::gfxInit() {
    // Init GLFW
    if (!glfwInit()) throw std::runtime_error("Unable to initialize GLFW!");

    // Window stuff
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

    // Create GLFW window
    gameWindow = glfwCreateWindow(1300, 768, "MC Game", NULL, NULL);

    if (gameWindow == NULL) {
        throw std::runtime_error("Unable to create GLFW window");
        glfwTerminate();
    }

    glfwMakeContextCurrent(gameWindow); // Initialize GLEW

    // Initialize GLEW
    glewExperimental=true; // Needed in core profile
    if (glewInit() != GLEW_OK) throw std::runtime_error("Unable to initialize GLEW");

    // Set some OpenGL settings
    // Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

    // Set background color
    glClearColor(.65f, .89f, 1.f, 1.f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(gameWindow, GLFW_STICKY_KEYS, GL_TRUE);
}

void Game::gameLoop() {
    // Debugger
    ChunkBorderDebugger chunkBorderDebugger(world);
    // ------------------------------
    
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwGetWindowSize(gameWindow, &windowWidth, &windowHeight);

        float aspect = (float)windowWidth / (float)windowHeight;

        player.updatePlayer(*this);
        world.update();

        // Update the plugins
        pluginManager.pluginFrameUpdate(*this);

        // 3d stuff
        auto viewMatrix = player.getCameraViewMatrix();
        auto viewProjection = player.getViewProjection(aspect);

        world.draw(viewProjection);

        if (player.showingDebug()) chunkBorderDebugger.draw(viewProjection);
        billboardManager.draw(player.getPositionRef(), viewMatrix, viewProjection);
        hudTextManager.draw(aspect);

        // Swap buffers
        glfwSwapBuffers(gameWindow);
        glfwPollEvents();
    } while (glfwGetKey(gameWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(gameWindow) == 0); 
}

Player& Game::getPlayerRef() {
    return player;
}

World& Game::getWorldRef() {
    return world;
}

PluginManager& Game::getPluginManagerRef() {
    return pluginManager;
}

BillboardManager& Game::getBillboardManagerRef() {
    return billboardManager;
}

HUDTextManager& Game::getHUDTextManagerRef() {
    return hudTextManager;
}

GLFWwindow* Game::getGLFWwindow() {
    return gameWindow;
}

int Game::getWindowWidth() const {
    return windowWidth;
}
int Game::getWindowHeight() const {
    return windowHeight;
}
