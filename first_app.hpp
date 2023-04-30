#pragma once

#include "sve_descriptors.hpp"
#include "sve_device.hpp"
#include "sve_game_object.hpp"
#include "sve_renderer.hpp"
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
    void loadGameObjects();

    SveWindow sveWindow{WIDTH, HEIGHT, "Soliloquy"};
    SveDevice sveDevice{sveWindow};
    SveRenderer sveRenderer{sveWindow, sveDevice};

    // note: order of declarations matters
    std::unique_ptr<SveDescriptorPool> globalPool{};
    SveGameObject::Map gameObjects;
};

}  // namespace sve