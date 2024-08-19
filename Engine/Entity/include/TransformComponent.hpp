#pragma once
#include "Component.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Engine {
    class TransformComponent : public Component {
        public:
        TransformComponent();
        TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
        ~TransformComponent();

        void SetPosition(const glm::vec3& position);
        void SetRotation(const glm::quat& rotation);
        void SetScale(const glm::vec3& scale);

        const glm::vec3& GetPosition() const;
        const glm::quat& GetRotation() const;
        const glm::vec3& GetScale() const;

        glm::mat4 GetTransformMatrix() const;

        private:
        glm::vec3 m_Position;
        glm::quat m_Rotation;
        glm::vec3 m_Scale;
        };
    }
