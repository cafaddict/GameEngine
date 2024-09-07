#include "VulkanDebugMessenger.hpp"
#include "VulkanInstance.hpp"
#include <VulkanRenderer.hpp>
#include <memory>

namespace Engine {
Renderer *Renderer::Create(GLFWwindow *window) {
    ENGINE_WARN("Vulkan Renderer Creation");
    return new VulkanRenderer_refac(window);
}

VulkanRenderer_refac::VulkanRenderer_refac(GLFWwindow *window) {
    SetWindow(window);
    Init();
}

void VulkanRenderer_refac::Init() {
    m_Instance = std::make_shared<VulkanInstance>();
    m_DebugMessenger = std::make_shared<VulkanDebugMessenger>(m_Instance);
}

void VulkanRenderer_refac::SetWindowResized(bool resized) {}

void VulkanRenderer_refac::SetWindowMinimized(bool minimized) {}

void VulkanRenderer_refac::WaitIdle() {}

void VulkanRenderer_refac::Draw() {}

} // namespace Engine