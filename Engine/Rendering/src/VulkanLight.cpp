#include "VulkanLight.hpp"
#include "Log.hpp"
namespace Engine {
VulkanLight::VulkanLight(glm::vec3 position, glm::vec3 color, float intensity)
    : m_Position(position), m_Color(color), m_Intensity(intensity) {}
VulkanLight::~VulkanLight() { ENGINE_WARN("VulkanLight Destructor"); }
} // namespace Engine