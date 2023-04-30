#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "sve_buffer.hpp"
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

struct GlobalUbo {
    glm::mat4 projectionView{1.f};
    glm::vec4 ambientColor{1.f, 1.f, 1.f, .02f};
    glm::vec3 lightPosition{-1.f};
    alignas(16) glm::vec4 lightColor{1.f};  // w is intensity, alignment for padding against vec3
};

FirstApp::FirstApp() {
    globalPool = SveDescriptorPool::Builder(sveDevice)
                     .setMaxSets(SveSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SveSwapChain::MAX_FRAMES_IN_FLIGHT)
                     .build();
    loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {  // synchronization of frames
    // ubo for each frame in flight
    std::vector<std::unique_ptr<SveBuffer>> uboBuffers(SveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<SveBuffer>(
            sveDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout = SveDescriptorSetLayout::Builder(sveDevice)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                               .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        SveDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{sveDevice, sveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    SveCamera camera{};

    auto viewerObject = SveGameObject::createGameObject();
    viewerObject.transform.translation = {0.f, 0.f, -2.5};
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
        camera.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, .1f, 100.f);

        if (auto commandBuffer = sveRenderer.beginFrame()) {
            int frameIndex = sveRenderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
                gameObjects};

            // update
            GlobalUbo ubo{};
            ubo.projectionView = camera.getProjection() * camera.getView();
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();  // manually flush memory due to indexed data

            // render
            sveRenderer.beginSwapChainRenderPass(commandBuffer);
            simpleRenderSystem.renderGameObjects(frameInfo);
            sveRenderer.endSwapChainRenderPass(commandBuffer);
            sveRenderer.endFrame();
        }
    }
    vkDeviceWaitIdle(sveDevice.device());
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<SveModel> sveModel /* = SveModel::createModelFromFile(sveDevice, "models/buddha.obj") */;
    // auto buddha = SveGameObject::createGameObject();
    // buddha.model = sveModel;
    // buddha.transform.translation = {.0f, .0f, 2.5f};
    // buddha.transform.rotation = glm::vec3{0.f, 0.f, glm::pi<float>()};  // flip bunny
    // buddha.transform.scale = glm::vec3{1.f};
    // gameObjects.push_back(std::move(buddha));

    sveModel = SveModel::createModelFromFile(sveDevice, "models/smooth_vase.obj");
    auto smooth_vase = SveGameObject::createGameObject();
    smooth_vase.model = sveModel;
    smooth_vase.transform.translation = {-.5f, .5f, 0.f};
    smooth_vase.transform.scale = glm::vec3{3.f};
    gameObjects.emplace(smooth_vase.getId(), std::move(smooth_vase));

    sveModel = SveModel::createModelFromFile(sveDevice, "models/bunny.obj");
    auto stanford_bunny = SveGameObject::createGameObject();
    stanford_bunny.model = sveModel;
    stanford_bunny.transform.translation = {1.f, .525f, 0.f};
    stanford_bunny.transform.rotation = glm::vec3{0.f, glm::pi<float>(), glm::pi<float>()};  // flip bunny
    stanford_bunny.transform.scale = glm::vec3{.5f};
    gameObjects.emplace(stanford_bunny.getId(), std::move(stanford_bunny));

    sveModel = SveModel::createModelFromFile(sveDevice, "models/quad.obj");
    auto floor = SveGameObject::createGameObject();
    floor.model = sveModel;
    floor.transform.translation = {.5f, .5f, 0.f};
    floor.transform.scale = glm::vec3{3.f, 1.f, 3.f};
    gameObjects.emplace(floor.getId(), std::move(floor));
}

}  // namespace sve