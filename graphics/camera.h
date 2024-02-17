#pragma once

#include "common.h"

namespace Graphics {
class Camera {
public:
    void Init(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, float fovDegree, float aspect, float nearZ, float farZ);

public:
    glm::mat4 GetViewMatrix(void) const;
    glm::mat4 GetProjectionMatrix(void) const;

public: // view
    glm::vec3 m_pos;
    glm::vec3 m_front;
    glm::vec3 m_up;
    float m_pitch {};
    float m_yaw {};

private: // projection
    float m_fov;
    float m_aspect;
    float m_nearZ;
    float m_farZ;
};
}
