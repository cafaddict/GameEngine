#pragma once
#include "glm/fwd.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
namespace Engine {
class Camera {
    public:
    Camera() = default;
    Camera(glm::mat4 view, glm::mat4 projection);
    ~Camera();
    glm::mat4 getView() { return m_View; }
    glm::mat4 getProjection() { return m_Projection; }
    glm::vec3 getPosition() { return m_Position; }
    glm::vec3 setPosition(glm::vec3 position) { m_Position = position; }
    void *getData() { return &m_View; }
    // struct {
    //     glm::mat4 view;
    //     glm::mat4 proj;
    // } m_Data;

    private:
    glm::mat4 m_View;
    glm::mat4 m_Projection;
    glm::vec3 m_Position;
    // Camera(VkDevice device, VkExtent2D extent);
    // ~Camera();
    // void updateCamera();
    // void setExtent(VkExtent2D extent);
    // void setCameraSpeed(float speed);
    // void setCameraPosition(float x, float y, float z);
    // void setCameraRotation(float x, float y, float z);
    // void setCameraZoom(float zoom);
    // void setCameraFOV(float fov);
    // void setCameraNearClip(float nearClip);
    // void setCameraFarClip(float farClip);
    // void setCameraLookAt(float x, float y, float z);
    // void setCameraUp(float x, float y, float z);
    // void setCameraRight(float x, float y, float z);
    // void setCameraFront(float x, float y, float z);
    // void setCameraViewMatrix(glm::mat4 view);
    // void setCameraProjectionMatrix(glm::mat4 projection);
    // void setCameraViewProjectionMatrix(glm::mat4 viewProjection);
    // void setCameraViewProjectionMatrix();
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection);
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection, glm::mat4 model);
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection, glm::mat4 model,
    //                                    glm::mat4 lightSpaceMatrix);
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection, glm::mat4 model,
    //                                    glm::mat4 lightSpaceMatrix, glm::mat4 lightView);
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection, glm::mat4 model,
    //                                    glm::mat4 lightSpaceMatrix, glm::mat4 lightView, glm::mat4 lightProjection);
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection, glm::mat4 model,
    //                                    glm::mat4 lightSpaceMatrix, glm::mat4 lightView, glm::mat4 lightProjection,
    //                                    glm::mat4 lightViewProjection);
    // void setCameraViewProjectionMatrix(glm::mat4 view, glm::mat4 projection, glm::mat4 model,
    //                                    glm::mat4 lightSpaceMatrix, glm::mat4 lightView, glm::mat4 lightProjection,
    //                                    glm::mat4 lightViewProjection, glm::mat4 lightSpaceMatrixViewProjection);
};
} // namespace Engine