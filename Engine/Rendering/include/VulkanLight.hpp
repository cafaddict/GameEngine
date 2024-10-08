#pragma once
#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
namespace Engine {
class VulkanLight {
    public:
    VulkanLight() = default;
    VulkanLight(glm::vec3 position, glm::vec3 color, float intensity);
    ~VulkanLight();
    // struct {
    //     glm::vec3 position;
    //     glm::vec3 color;
    //     float intensity;
    // } m_Data;
    glm::vec3 m_Position;
    glm::vec3 m_Color;

    private:
    float m_Intensity;
};
} // namespace Engine