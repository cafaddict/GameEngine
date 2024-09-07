#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <Log.hpp>
#include <Rendering.hpp>
#include <VulkanDebugMessenger.hpp>
#include <VulkanInstance.hpp>

#include <memory>

namespace Engine {
class VulkanRenderer_refac : public Renderer {
    private:
    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanDebugMessenger> m_DebugMessenger;
    GLFWwindow *m_Window = nullptr;

    public:
    // Constructor that initializes the window and Vulkan objects
    VulkanRenderer_refac(GLFWwindow *window);

    // Destructor
    virtual ~VulkanRenderer_refac();

    // Pure virtual functions from Renderer that must be overridden
    virtual void Draw() override;
    virtual void SetWindow(GLFWwindow *window) override { m_Window = window; }
    virtual void SetWindowResized(bool resized) override;
    virtual void SetWindowMinimized(bool minimized) override;
    virtual void WaitIdle() override;
    virtual void addModel(std::string model_path) override;

    private:
    // Initializes Vulkan instance and debug messenger
    virtual void Init() override;
};
} // namespace Engine