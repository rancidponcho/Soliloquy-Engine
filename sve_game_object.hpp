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
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                scale.x * (c1 * c3 + s1 * s2 * s3),
                scale.x * (c2 * s3),
                scale.x * (c1 * s2 * s3 - c3 * s1),
                0.f,
            },
            {
                scale.y * (c3 * s1 * s2 - c1 * s3),
                scale.y * (c2 * c3),
                scale.y * (c1 * c3 * s2 + s1 * s3),
                0.f,
            },
            {
                scale.z * (c2 * s1),
                scale.z * (-s2),
                scale.z * (c1 * c2),
                0.f,
            },
            {translation.x, translation.y, translation.z, 1.f},
        };
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