#pragma once

#include "common.h"

namespace Graphics {
class Transform {
public:
    Transform() = default;
    ~Transform() = default;

public:
    glm::mat4 GetWorldMatrix(void) const;
    void SetRotation(const glm::vec4& quaternion);

public:
    glm::vec3 m_pos { 0.0f };
    glm::vec3 m_rotation { 0.0f };
    glm::vec3 m_scale { 10.0f };
};
}
