#include "VulkanCamera.hpp"
#include "Log.hpp"
namespace Engine {
VulkanCamera::VulkanCamera(glm::mat4 view, glm::mat4 projection) : m_View(view), m_Projection(projection) {
    // m_Data.view = view;
    // m_Data.proj = projection;
}
VulkanCamera::~VulkanCamera() { ENGINE_WARN("VulkanCamera Destructor"); }
} // namespace Engine