#include "VulkanCamera.hpp"
namespace Engine {
VulkanCamera::VulkanCamera(glm::mat4 view, glm::mat4 projection) : m_View(view), m_Projection(projection) {}
VulkanCamera::~VulkanCamera() {}
} // namespace Engine