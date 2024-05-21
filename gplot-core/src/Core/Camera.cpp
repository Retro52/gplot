#include <Core/Camera.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

using namespace gplot::core;

Camera::Camera(float fov)
    : m_fov(fov)
{
    m_front = glm::vec3(0, 0, -1);
    m_right = glm::vec3(1, 0, 0);
    m_up    = glm::vec3(0, 1, 0);
}

Camera::Camera(float fov, const glm::vec2& aspect_ratio)
    : Camera(fov)
{
    m_aspect_ratio = aspect_ratio;
}

void Camera::SetRotation(const glm::vec3& rotation)
{
    auto model = glm::mat4(1.0f);

    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));

    m_up = glm::vec3(model * glm::vec4(0, 1, 0, 1));
    m_front = glm::vec3(model * glm::vec4(0, 0, -1, 1));
    m_right = glm::vec3(model * glm::vec4(1, 0, 0, 1));
}

[[nodiscard]] glm::mat4 Camera::GetView()
{
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

[[nodiscard]] glm::mat4 Camera::GetProjection() const
{
    return glm::perspective(m_fov, m_aspect_ratio.x / m_aspect_ratio.y, m_near_z, m_far_z);
}
