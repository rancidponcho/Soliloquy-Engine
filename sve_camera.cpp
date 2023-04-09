#include "sve_camera.hpp"

// libs

// std
#include <cassert>
#include <limits>

namespace sve {

void SveCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
    // reset projection
    projectionMatrix = glm::mat4{1.f};
    // set ortho
    projectionMatrix[0][0] = 2.f / (right - left);
    projectionMatrix[1][1] = 2.f / (bottom - top);
    projectionMatrix[2][2] = 1.f / (far - near);
    projectionMatrix[3][0] = -(right + left) / (right - left);
    projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
    projectionMatrix[3][2] = -near / (far - near);
}

void SveCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);  // precision check
    // reset projection
    projectionMatrix = glm::mat4{0.f};
    // set simplified perspective mat
    const float tanHalfFovy = tan(fovy / 2.f);  // simplified prep
    projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
    projectionMatrix[1][1] = 1.f / (tanHalfFovy);
    projectionMatrix[2][2] = far / (far - near);
    projectionMatrix[2][3] = 1.f;
    projectionMatrix[3][2] = -(far * near) / (far - near);
}

void SveCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    // reset view
    viewMatrix = glm::mat4{1.f};
    // compute coordinate basis vectors and store in matrix columns // my implementation
    // viewMatrix[2] = glm::vec4(glm::normalize(direction), 0.f);
    // viewMatrix[0] = glm::vec4(glm::normalize(glm::cross(glm::vec3(viewMatrix[2]), up)), 0.f);
    // viewMatrix[1] = glm::vec4(glm::cross(glm::vec3(viewMatrix[2]), glm::vec3(viewMatrix[0])), 0.f);
    // viewMatrix[3] = glm::vec4(position, 1.f);
    // viewMatrix = glm::transpose(viewMatrix);
    // long implementation
    const glm::vec3 w{glm::normalize(direction)};
    const glm::vec3 u{glm::normalize(glm::cross(w, up))};
    const glm::vec3 v{glm::cross(w, u)};

    viewMatrix[0][0] = u.x;
    viewMatrix[1][0] = u.y;
    viewMatrix[2][0] = u.z;
    viewMatrix[0][1] = v.x;
    viewMatrix[1][1] = v.y;
    viewMatrix[2][1] = v.z;
    viewMatrix[0][2] = w.x;
    viewMatrix[1][2] = w.y;
    viewMatrix[2][2] = w.z;
    viewMatrix[3][0] = -glm::dot(u, position);
    viewMatrix[3][1] = -glm::dot(v, position);
    viewMatrix[3][2] = -glm::dot(w, position);
}

void SveCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
}

void SveCamera::setViewXYZ(glm::vec3 position, glm::vec3 rotation) {
    const float c3 = glm::cos(rotation.z);
    const float s3 = glm::sin(rotation.z);
    const float c2 = glm::cos(rotation.x);
    const float s2 = glm::sin(rotation.x);
    const float c1 = glm::cos(rotation.y);
    const float s1 = glm::sin(rotation.y);
    // tait-bryan order
    const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
    const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
    const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};

    // set matrix // my implementation
    viewMatrix[0] = glm::vec4{u, 0.f};
    viewMatrix[1] = glm::vec4{v, 0.f};
    viewMatrix[2] = glm::vec4{w, 0.f};
    viewMatrix[3] = glm::vec4{-glm::dot(u, position), -glm::dot(v, position), -glm::dot(w, position), 1.f};
    // long implementation
    // viewMatrix[0][0] = u.x;
    // viewMatrix[1][0] = u.y;
    // viewMatrix[2][0] = u.z;
    // viewMatrix[0][1] = v.x;
    // viewMatrix[1][1] = v.y;
    // viewMatrix[2][1] = v.z;
    // viewMatrix[0][2] = w.x;
    // viewMatrix[1][2] = w.y;
    // viewMatrix[2][2] = w.z;
    // viewMatrix[3][0] = -glm::dot(u, position);
    // viewMatrix[3][1] = -glm::dot(v, position);
    // viewMatrix[3][2] = -glm::dot(w, position);
}

}  // namespace sve