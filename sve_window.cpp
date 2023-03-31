#include "sve_window.hpp"

// std
#include <stdexcept>

namespace sve {

SveWindow::SveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow();
}

SveWindow::~SveWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void SveWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void SveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

}  // namespace sve