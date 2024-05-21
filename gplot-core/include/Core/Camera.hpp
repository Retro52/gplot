#pragma once

#include <iostream>
#include <glm/glm.hpp>

namespace gplot::core
{
    class Camera
    {
    public:
        explicit Camera(float fov);

        explicit Camera(float fov, const glm::vec2& aspect_ratio);

        /**
         * Updates camera internal matrices and vectors, must be called before getting view and projection matrices
         * @param rotation camera rotation
         */
        void SetRotation(const glm::vec3& rotation);

        [[nodiscard]] glm::mat4 GetView();

        [[nodiscard]] glm::mat4 GetProjection() const;

        [[nodiscard]] inline float GetFieldOfView() const { return m_fov; };

        [[nodiscard]] inline float GetFarPlane()  const { return m_far_z; }

        [[nodiscard]] inline float GetNearPlane() const { return m_near_z; }

        [[nodiscard]] inline const glm::vec3& GetUpVector() const { return m_up; };

        [[nodiscard]] inline const glm::vec3& GetFrontVector() const { return m_front; };

        [[nodiscard]] inline const glm::vec3& GetRightVector() const { return m_right; };

        [[nodiscard]] inline float GetAspectRatioFloat() const { return m_aspect_ratio.x / m_aspect_ratio.y; }

    public:

        glm::vec3 m_pos { 0 };
        glm::vec2 m_aspect_ratio { 1, 1 };

        float m_fov { glm::radians(60.0F) };

        float m_near_z { 0.005F };
        float m_far_z { 2097152.0F };

    private:

        glm::vec3 m_up { 0 };
        glm::vec3 m_right { 0 };
        glm::vec3 m_front { 0 };
    };
}
