#include "Light.hpp"
#include "Log.hpp"
namespace Engine {
Light::Light(glm::vec3 position, glm::vec3 color, float intensity)
    : m_Position(position), m_Color(color), m_Intensity(intensity) {}
Light::~Light() { ENGINE_WARN("Light Destructor"); }
} // namespace Engine