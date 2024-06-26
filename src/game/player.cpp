#include "player.hpp"

#include <cstdio>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window.hpp"

const float mouseSens = .005f;
const float moveSpeed = .05f;

Player::Player(glm::vec3 _position, glm::vec3 _looking, float _fov):
    position(_position),
    looking(_looking),
    settings({ .fov = _fov, .showDebug = false, .mouseLookFocus = true}) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Player::~Player() {}

void Player::updatePlayer(float deltaTime) {
    static double pitch = 2.8f;
    static double yaw = 0;
    static double mx, my = 0.f;

    int fwidth;
    int fheight;
    glfwGetWindowSize(window, &fwidth, &fheight);

    // Half screen width and heigh
    const int hwidth = fwidth / 2;
    const int hheight = fheight / 2;

    if (settings.mouseLookFocus) {
        // Get screen width and height

        // First, do the player look code

        glfwGetCursorPos(window, &mx, &my);

        // Subtract the halfWidth and halfHeight to get a mouse movement amount based around (0, 0)
        mx -= hwidth;
        my -= hheight;

        // Set the cursor back to the middle of the frame
        glfwSetCursorPos(window, hwidth, hheight);
        
        // Yaw is the left to right movement of the camera
        // mouseSens is a scalar to control move speed
        yaw += mx * mouseSens;

        // Lock the camera to almost all the way down and almost all the way up
        pitch = std::min(1.5f*M_PI - .01, std::max((double)M_PI_2 + .01, pitch + my * mouseSens));

        // Calculate the look vector
        looking = glm::vec3(cos(pitch)*cos(yaw), sin(pitch), sin(yaw) * cos(pitch));
    }

    // Now player movement
    // Forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += looking * moveSpeed * deltaTime;
    }
    // Back
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= looking * moveSpeed * deltaTime;
    }
    // Left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position += glm::normalize(glm::vec3(looking.z, 0, -looking.x)) * moveSpeed * deltaTime;
    }
    // Right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position -= glm::normalize(glm::vec3(looking.z, 0, -looking.x)) * moveSpeed * deltaTime;
    }
    // Up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        position += glm::vec3(0, 1, 0) * moveSpeed * deltaTime;
    }
    // Down
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        position -= glm::vec3(0, 1, 0) * moveSpeed * deltaTime;
    }

    // Show wireframe toggle
    static bool wireframeButtonWasPressed = false;
    static bool wireframeEnabled = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (wireframeButtonWasPressed == false) {
            wireframeButtonWasPressed = true;
            
            // Toggle wireframe
            wireframeEnabled = !wireframeEnabled;
            if (wireframeEnabled) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    } else {
        wireframeButtonWasPressed = false;
    }

    // Break block controller
    static bool breakButtonWasPressed = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (breakButtonWasPressed == false) {
            breakButtonWasPressed = true;
            
            // Cast a ray and try to break the block we are looking at
            // Ray r(position, looking);
            // r.tryBreakBlock(game.getWorldRef(), 30);
        }
    } else {
        breakButtonWasPressed = false;
    }

    // Place block controller
    static bool placeButtonWasPressed = false;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (placeButtonWasPressed == false) {
            placeButtonWasPressed = true;
            
            // Cast a ray and try to break the block we are looking at
            // Ray r(position, looking);
            // r.tryPlaceBlock(game.getWorldRef(), Block(dirtIndex));
        }
    } else {
        placeButtonWasPressed = false;
    }

    static bool debugButtonWasPressed = false;
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (debugButtonWasPressed == false) {
            debugButtonWasPressed = true;

            settings.showDebug = !settings.showDebug;
        }
    } else {
        debugButtonWasPressed = false;
    }

    static bool mouseUnlockWasPressed = false;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (mouseUnlockWasPressed == false) {
            mouseUnlockWasPressed = true;

            if (settings.mouseLookFocus)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetCursorPos(window, hwidth, hheight);
            }

            settings.mouseLookFocus = !settings.mouseLookFocus;
        }
    } else {
        mouseUnlockWasPressed = false;
    }

    static bool createSavePressed = false;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        if (createSavePressed == false) {
            createSavePressed = true;

            // game.getWorldRef().saveWorld("test");
        }
    } else {
        createSavePressed = false;
    }
}

glm::mat4 Player::getCameraProjectionMatrix() const {
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);
    float aspect = (float)width / (float)height;
    return glm::perspective(glm::radians(settings.fov), aspect, 0.01f, 1000.0f);
}

glm::mat4 Player::getCameraViewMatrix() const {
    return glm::lookAt(
		position, // Camera is at (4,3,3), in World Space
		position + looking, // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

glm::mat4 Player::getViewProjection() const {
    auto projection = getCameraProjectionMatrix();
    auto view = getCameraViewMatrix();
    return projection * view;
}


bool Player::showingDebug() const {
    return settings.showDebug;
}

glm::vec3& Player::getLookingVectorRef() {
    return looking;
}

glm::vec3& Player::getPositionRef() {
    return position;
}