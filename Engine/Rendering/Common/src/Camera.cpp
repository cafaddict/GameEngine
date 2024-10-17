#include "Camera.hpp"
#include "Log.hpp"
namespace Engine {
Camera::Camera(glm::mat4 view, glm::mat4 projection) : m_View(view), m_Projection(projection) {
    // m_Data.view = view;
    // m_Data.proj = projection;
}

Camera::Camera(glm::mat4 view, glm::mat4 projection, glm::vec3 position)
    : m_View(view), m_Projection(projection), m_Position(position) {
    // m_Data.view = view;
    // m_Data.proj = projection;
}
Camera::~Camera() { ENGINE_WARN("Camera Destructor"); }
} // namespace Engine