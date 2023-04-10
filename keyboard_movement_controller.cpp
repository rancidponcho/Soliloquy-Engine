#include "keyboard_movement_controller.hpp"

namespace sve {

void KeyboardMovementController::moveInPlaneXZ(GLFWwindow *window, float dt, SveGameObject &gameObject) {
    glm::vec3 rotate{0};
    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {        // if rotate is not zero
        gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);  // normalized so that diagonal movement is not faster
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -glm::half_pi<float>(), glm::half_pi<float>());  // clamp pitch
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());                             // wrap yaw

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
    const glm::vec3 upDir{0.f, -1.f, 0.f};

    glm::vec3 moveDir{0};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {          // if moveDir is not zero
        gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);  // normalized so that diagonal movement is not faster
    }
}

}  // namespace sve
