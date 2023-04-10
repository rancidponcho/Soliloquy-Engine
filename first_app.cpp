#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "sve_camera.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>  // for PI

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace sve {

FirstApp::FirstApp() { loadGameObjects(); }

FirstApp::~FirstApp() {}

void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{sveDevice, sveRenderer.getSwapChainRenderPass()};
    SveCamera camera{};

    auto viewerObject = SveGameObject::createGameObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    while (!sveWindow.shouldClose()) {
        glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        // frameTime = glm::min(frameTime, MAX_FRAME_TIME);

        cameraController.moveInPlaneXZ(sveWindow.getGLFWwindow(), frameTime, viewerObject);
        camera.setViewXYZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = sveRenderer.getAspectRatio();  // prevent resize warping
        camera.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, .1f, 10.f);

        if (auto commandBuffer = sveRenderer.beginFrame()) {
            sveRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
            sveRenderer.endSwapChainRenderPass(commandBuffer);
            sveRenderer.endFrame();
        }
    }
    vkDeviceWaitIdle(sveDevice.device());
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<SveModel> sveModel = SveModel::createModelFromFile(sveDevice, "models/smooth_vase.obj");

    auto gameObj = SveGameObject::createGameObject();
    gameObj.model = sveModel;
    gameObj.transform.translation = {0.0f, 0.0f, 2.5f};
    gameObj.transform.scale = glm::vec3{3.f};
    gameObjects.push_back(std::move(gameObj));
}

}  // namespace sve