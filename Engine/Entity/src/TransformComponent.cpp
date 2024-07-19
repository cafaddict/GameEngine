#include "TransformComponent.hpp"


namespace Engine {

    TransformComponent::TransformComponent()
        : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(glm::quat()), m_Scale(1.0f, 1.0f, 1.0f) {
        }

    TransformComponent::~TransformComponent()
        {

        }

    TransformComponent::TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
        : m_Position(position), m_Rotation(rotation), m_Scale(scale) {
        }

    void TransformComponent::SetPosition(const glm::vec3& position) {
        m_Position = position;
        }

    void TransformComponent::SetRotation(const glm::quat& rotation) {
        m_Rotation = rotation;
        }

    void TransformComponent::SetScale(const glm::vec3& scale) {
        m_Scale = scale;
        }

    const glm::vec3& TransformComponent::GetPosition() const {
        return m_Position;
        }

    const glm::quat& TransformComponent::GetRotation() const {
        return m_Rotation;
        }

    const glm::vec3& TransformComponent::GetScale() const {
        return m_Scale;
        }

    glm::mat4 TransformComponent::GetTransformMatrix() const {
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_Position);
        glm::mat4 rotationMatrix = glm::toMat4(m_Rotation);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), m_Scale);

        return translationMatrix * rotationMatrix * scaleMatrix;
        }
    }
