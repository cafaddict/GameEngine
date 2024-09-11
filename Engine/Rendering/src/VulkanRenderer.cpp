#include "Log.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanInstance.hpp"
#include <VulkanRenderer.hpp>
#include <memory>

namespace Engine {

Renderer *Renderer::Create(GLFWwindow *window) {
    ENGINE_WARN("Vulkan Renderer Creation");
    return new VulkanRenderer_refac(window);
}

// Constructor implementation
VulkanRenderer_refac::VulkanRenderer_refac(GLFWwindow *window) {
    SetWindow(window);
    Init(); // Initialize Vulkan instance and debug messenger
}

// Destructor implementation
VulkanRenderer_refac::~VulkanRenderer_refac() {
    // Vulkan cleanup (if needed) happens automatically due to shared_ptr destructors
}

// Init implementation to set up Vulkan instance and debug messenger
void VulkanRenderer_refac::Init() {
    ENGINE_WARN("Vulkan Renderer Init");
    m_Instance = std::make_shared<VulkanInstance>();
    ENGINE_WARN("Vulkan Instance Created");
    m_DebugMessenger = std::make_shared<VulkanDebugMessenger>(m_Instance);
    ENGINE_WARN("Vulkan Debug Messenger Created");
    m_Device = std::make_shared<VulkanDevice>(m_Instance, m_Window);
    ENGINE_WARN("Vulkan Device Created");
    m_SwapChain = std::make_shared<VulkanSwapChain>(m_Device, m_Window);
    ENGINE_WARN("Vulkan Swap Chain Created");
    m_RenderPass = std::make_shared<VulkanRenderPass>(m_Device, m_SwapChain);
    ENGINE_WARN("Vulkan Render Pass Created");
    m_FrameBuffer = std::make_shared<VulkanFrameBuffer>(m_Device, m_SwapChain, m_RenderPass);
    ENGINE_WARN("Vulkan Frame Buffer Created");
}

// Implement pure virtual function from Renderer
void VulkanRenderer_refac::SetWindowResized(bool resized) {
    // Handle window resize logic
}

// Implement pure virtual function from Renderer
void VulkanRenderer_refac::SetWindowMinimized(bool minimized) {
    // Handle window minimized logic
}

// Implement pure virtual function from Renderer
void VulkanRenderer_refac::WaitIdle() {
    // Implement Vulkan logic to wait for GPU operations to finish
}

// Implement pure virtual function from Renderer
void VulkanRenderer_refac::Draw() {
    // Implement Vulkan draw logic here
}

} // namespace Engine