#pragma once

#include "sve_model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace sve {

struct TransformComponent {
    glm::vec3 translation{};  // position offset
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};

    // Matrix corresponds to translate * Rz * Ry * Rx * scale transformation
    // Rotation convention uses Tait-Bryan angles with axis order Y(1), X(2), Z(3)
    glm::mat4 mat4() {
        auto transform = glm::translate(glm::mat4{1.f}, translation);
        transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f});
        transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f});
        transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f});
        transform = glm::scale(transform, scale);
        return transform;
    }
};

// eventually use entity component system
class SveGameObject {
   public:
    using id_t = unsigned int;

    static SveGameObject createGameObject() {
        static id_t currentId = 0;
        return SveGameObject(currentId++);
    }

    SveGameObject(const SveGameObject &) = delete;
    SveGameObject &operator=(const SveGameObject &) = delete;
    SveGameObject(SveGameObject &&) = default;
    SveGameObject &operator=(SveGameObject &&) = default;

    id_t getId() { return id; }

    std::shared_ptr<SveModel> model;  // shared model among all game objects
    glm::vec3 color{};
    TransformComponent transform{};

   private:
    SveGameObject(id_t objId) : id(objId) {}  // private constructor to force use of factory method

    id_t id;
};

}  // namespace sve