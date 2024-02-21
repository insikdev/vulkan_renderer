#include "transform.h"

glm::mat4 Graphics::Transform::GetWorldMatrix(void) const
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_pos);
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

    return translation * rotationX * rotationY * rotationZ * scale;
}

void Graphics::Transform::SetRotation(const glm::vec4& quaternion)
{
    m_rotation = glm::eulerAngles(glm::quat { quaternion });
}
