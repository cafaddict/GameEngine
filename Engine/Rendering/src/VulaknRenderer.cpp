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
    m_Instance = std::make_shared<VulkanInstance>();
    m_DebugMessenger = std::make_shared<VulkanDebugMessenger>(m_Instance);
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

// Implement pure virtual function from Renderer (empty for now)
void VulkanRenderer_refac::addModel(std::string model_path) {
    // Logic to load a model can be added here
}

// Implement pure virtual function from Renderer
void VulkanRenderer_refac::Draw() {
    // Implement Vulkan draw logic here
}

} // namespace Engine