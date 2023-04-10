#include "sve_model.hpp"

#include "sve_utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace std {

template <>
struct hash<sve::SveModel::Vertex> {
    size_t operator()(sve::SveModel::Vertex const& vertex) const {
        size_t seed = 0;
        sve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
        return seed;
    }
};

}  // namespace std

namespace sve {

SveModel::SveModel(SveDevice& device, const SveModel::Builder& builder) : sveDevice{device} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

SveModel::~SveModel() {
    vkDestroyBuffer(sveDevice.device(), vertexBuffer, nullptr);
    vkFreeMemory(sveDevice.device(), vertexBufferMemory, nullptr);

    if (hasIndexbuffer) {
        vkDestroyBuffer(sveDevice.device(), indexBuffer, nullptr);
        vkFreeMemory(sveDevice.device(), indexBufferMemory, nullptr);
    }
}

std::unique_ptr<SveModel> SveModel::createModelFromFile(SveDevice& device, const std::string& path) {
    Builder builder{};
    builder.loadModel(path);
    std::cout << "Loaded model: " << path << std::endl;
    std::cout << "Vertex count: " << builder.vertices.size() << std::endl;
    return std::make_unique<SveModel>(device, builder);
}

void SveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3.");
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    sveDevice.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    void* data;
    vkMapMemory(sveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(sveDevice.device(), stagingBufferMemory);

    sveDevice.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexBufferMemory);

    sveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(sveDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(sveDevice.device(), stagingBufferMemory, nullptr);
}

void SveModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
    if (indices.empty()) {
        return;
    }
    hasIndexbuffer = true;
    indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    sveDevice.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    void* data;
    vkMapMemory(sveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(sveDevice.device(), stagingBufferMemory);

    sveDevice.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBuffer,
        indexBufferMemory);

    sveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

    vkDestroyBuffer(sveDevice.device(), stagingBuffer, nullptr);
    vkFreeMemory(sveDevice.device(), stagingBufferMemory, nullptr);
}

void SveModel::draw(VkCommandBuffer commandBuffer) {
    if (hasIndexbuffer) {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}

void SveModel::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (hasIndexbuffer) {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);  // 32 bit indices => | 2^16 -1 = 65535 | 2^32 -1 = 4294967295 | 2^64 -1 = 18446744073709551615 | etc...
    }
}

std::vector<VkVertexInputBindingDescription> SveModel::Vertex::getBindingDescriptions() {
    return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
}

std::vector<VkVertexInputAttributeDescription> SveModel::Vertex::getAttributeDescriptions() {
    // binding, location, format, offset
    return {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
    };
}

void SveModel::Builder::loadModel(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            if (index.vertex_index >= 0) {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };

                // set color
                auto colorIndex = 3 * index.vertex_index + 2;
                if (colorIndex < attrib.colors.size()) {
                    vertex.color = {
                        attrib.colors[colorIndex - 2],
                        attrib.colors[colorIndex - 1],
                        attrib.colors[colorIndex - 0],
                    };
                } else {
                    vertex.color = {1.f, 1.f, 1.f};  // default color
                }
            }

            if (index.normal_index >= 0) {
                vertex.normal = {
                    attrib.normals[3 * index.vertex_index + 0],
                    attrib.normals[3 * index.vertex_index + 1],
                    attrib.normals[3 * index.vertex_index + 2],
                };
            }

            if (index.texcoord_index >= 0) {
                vertex.uv = {
                    attrib.texcoords[2 * index.vertex_index + 0],
                    1.0f - attrib.texcoords[2 * index.vertex_index + 1],
                };
            }

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }
}

}  // namespace sve