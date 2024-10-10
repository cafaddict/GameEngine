#pragma once

#include "glm/fwd.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
namespace Engine {
class Light {
    public:
    Light() = default;
    Light(glm::vec3 position, glm::vec3 color, float intensity);
    ~Light();
    // struct {
    //     glm::vec3 position;
    //     glm::vec3 color;
    //     float intensity;
    // } m_Data;
    void *getData() { return &m_Position; }
    glm::vec3 getPosition() { return m_Position; }
    glm::vec3 getColor() { return m_Color; }
    float getIntensity() { return m_Intensity; }

    private:
    glm::vec3 m_Position;
    glm::vec3 m_Color;
    float m_Intensity;
};
} // namespace Engine