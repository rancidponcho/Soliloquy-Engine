#pragma once

#include "sve_camera.hpp"
#include "sve_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace sve {

struct FrameInfo {
    int framIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    SveCamera camera;
    VkDescriptorSet globalDescriptorSet;
    SveGameObject::Map &gameObjects;
};

}  // namespace sve
