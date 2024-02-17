#include "camera.h"

void Graphics::Camera::Init(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up, float fovDegree, float aspect, float nearZ, float farZ)
{
    m_pos = pos;
    m_front = front;
    m_up = up;
    m_fov = fovDegree;
    m_aspect = aspect;
    m_nearZ = nearZ;
    m_farZ = farZ;
}

glm::mat4 Graphics::Camera::GetViewMatrix(void) const
{
    glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 front = rotateY * rotateX * glm::vec4(m_front, 0.0f);

    return glm::lookAtLH(m_pos, m_pos + front, m_up);
}

glm::mat4 Graphics::Camera::GetProjectionMatrix(void) const
{
    return glm::perspectiveLH(glm::radians(m_fov), m_aspect, m_nearZ, m_farZ);
}
