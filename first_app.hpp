#pragma once

#include "sve_device.hpp"
#include "sve_pipeline.hpp"
#include "sve_swap_chain.hpp"
#include "sve_window.hpp"

// std
#include <memory>
#include <vector>

namespace sve {
class FirstApp {
   public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp &) = delete;
    FirstApp &operator=(const FirstApp &) = delete;

    void run();

   private:
   void createPipelineLayout();
   void createPipeline();
   void createCommandBuffers();
   void drawFrame();

    SveWindow sveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    SveDevice sveDevice{sveWindow};
    SveSwapChain sveSwapChain{sveDevice, sveWindow.getExtent()};
    std::unique_ptr<SvePipeline> svePipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};

}  // namespace sve