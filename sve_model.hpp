#pragma once

#include "sve_buffer.hpp"
#include "sve_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace sve {

class SveModel {
   public:
    struct Vertex {
        // interleving
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const Vertex &other) const {
            return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
        }
    };

    struct Builder {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        void loadModel(const std::string &path);
    };

    SveModel(SveDevice &device, const SveModel::Builder &builder);
    ~SveModel();
    SveModel(const SveModel &) = delete;
    SveModel &operator=(const SveModel &) = delete;

    static std::unique_ptr<SveModel> createModelFromFile(SveDevice &device, const std::string &path);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

   private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    SveDevice &sveDevice;

    std::unique_ptr<SveBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexbuffer = false;
    std::unique_ptr<SveBuffer> indexBuffer;
    uint32_t indexCount;
};

}  // namespace sve