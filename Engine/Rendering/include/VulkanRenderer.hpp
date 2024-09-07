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
    VulkanRenderer_refac();
    VulkanRenderer_refac(GLFWwindow *window);
    ~VulkanRenderer_refac();
    virtual void Draw() override;
    void SetWindow(GLFWwindow *window) override { m_Window = window; };
    virtual void SetWindowResized(bool resized) override;
    virtual void SetWindowMinimized(bool minimized) override;
    virtual void WaitIdle() override;

    private:
    virtual void Init() override;
};
} // namespace Engine