#pragma once

#include "sve_game_object.hpp"
#include "sve_window.hpp"

namespace sve {

class KeyboardMovementController {
   public:
    struct KeyMappings {
        // move controls
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_SHIFT;
        int rollLeft = GLFW_KEY_Q;
        int rollRight = GLFW_KEY_E;
        // look controls
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
    };

    void moveInPlaneXZ(GLFWwindow* window, float dt, SveGameObject& gameObject);

    KeyMappings keys{};
    float moveSpeed{3.f};
    float lookSpeed{1.5f};
};

}  // namespace sve
