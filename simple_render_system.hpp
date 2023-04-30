#pragma once

#include "sve_camera.hpp"
#include "sve_device.hpp"
#include "sve_frame_info.hpp"
#include "sve_game_object.hpp"
#include "sve_pipeline.hpp"
#include "sve_renderer.hpp"
#include "sve_window.hpp"

// std
#include <memory>
#include <vector>

namespace sve {
class SimpleRenderSystem {
   public:
    SimpleRenderSystem(SveDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(FrameInfo &frameInfo);

   private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    SveDevice &sveDevice;

    std::unique_ptr<SvePipeline> svePipeline;
    VkPipelineLayout pipelineLayout;
};

}  // namespace sve