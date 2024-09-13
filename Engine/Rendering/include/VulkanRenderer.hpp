#pragma once
#include "VulkanBuffer.hpp"
#include <_types/_uint32_t.h>
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <Log.hpp>
#include <Rendering.hpp>
#include <VulkanDebugMessenger.hpp>
#include <VulkanInstance.hpp>
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanFrameBuffer.hpp"
#include "EntityManager.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanSyncObject.hpp"

#include <memory>

namespace Engine {
class VulkanRenderer_refac : public Renderer {
    private:
    std::shared_ptr<EntityManager> m_EntityManager;
    std::unordered_map<VulkanShadersData, VulkanGraphicsPipeline, ShaderDataHash> m_PipelineCache;
    std::unordered_map<std::shared_ptr<Entity>, VulkanGraphicsPipeline> m_EntityPipelines;

    std::shared_ptr<VulkanInstance> m_Instance;
    std::shared_ptr<VulkanDebugMessenger> m_DebugMessenger;
    GLFWwindow *m_Window = nullptr;
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanSwapChain> m_SwapChain;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;
    std::shared_ptr<VulkanFrameBuffer> m_FrameBuffer;
    std::shared_ptr<VulkanCommandBuffer> m_CommandBuffer;
    std::shared_ptr<VulkanCommandBuffer> m_ComputeCommandBuffer;
    std::shared_ptr<VulkanCommandBuffer> m_TransferCommandBuffer;
    std::shared_ptr<VulkanSemaphore> m_ImageAvailableSemaphore;
    std::shared_ptr<VulkanSemaphore> m_RenderFinishedSemaphore;
    std::shared_ptr<VulkanSemaphore> m_ComputeFinishedSemaphore;
    std::shared_ptr<VulkanSemaphore> m_TransferFinishedSemaphore;
    std::shared_ptr<VulkanFence> m_InFlightFences;
    std::shared_ptr<VulkanFence> m_ComputeFences;
    uint32_t m_CurrentFrame = 0;
    uint32_t m_ImageIndex = 0;

    std::shared_ptr<VulkanVertexBuffer> m_VertexBuffer;
    std::shared_ptr<VulkanIndexBuffer> m_IndexBuffer;
    std::shared_ptr<VulkanUniformBuffer> m_UniformBuffers;

    public:
    // Constructor that initializes the window and Vulkan objects
    VulkanRenderer_refac(GLFWwindow *window);
    void createEntityResources();

    // Destructor
    virtual ~VulkanRenderer_refac();

    // Pure virtual functions from Renderer that must be overridden
    virtual void Draw() override;
    virtual void BeginRecord() override;
    virtual void EndRecord() override;
    virtual void SetWindow(GLFWwindow *window) override { m_Window = window; }
    virtual void SetWindowResized(bool resized) override;
    virtual void SetWindowMinimized(bool minimized) override;
    virtual void WaitIdle() override;
    void SetEntityManager(std::shared_ptr<EntityManager> entitymanager) { m_EntityManager = entitymanager; }

    private:
    // Initializes Vulkan instance and debug messenger
    virtual void Init() override;
};
} // namespace Engine