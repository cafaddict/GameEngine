#pragma once
#include "Camera.hpp"
#include "Light.hpp"
#include "TextureData.hpp"
#include "VulkanDescriptorSet.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanVertex.hpp"
#include "glm/fwd.hpp"
#include <_types/_uint32_t.h>
#include <sys/types.h>
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "EntityManager.hpp"
#include "VulkanCamera.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanLight.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanSyncObject.hpp"
#include <Log.hpp>
#include <Rendering.hpp>
#include <VulkanDebugMessenger.hpp>
#include <VulkanInstance.hpp>

#include <memory>

namespace Engine {
class VulkanRenderer : public Renderer {
    private:
    std::shared_ptr<EntityManager> m_EntityManager;
    std::unordered_map<VulkanShadersData, std::shared_ptr<VulkanGraphicsPipeline>, ShaderDataHash> m_PipelineCache;
    std::unordered_map<std::shared_ptr<const TextureData>, std::shared_ptr<VulkanDescriptorSet>, DescriptorSetDataHash>
        m_DescriptorSetCache;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<VulkanGraphicsPipeline>> m_EntityPipelines;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<VulkanDescriptorSet>> m_EntityDescriptorSets;

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
    bool m_Minimizied = false;
    bool m_Resized = false;
    bool m_EntityUpdate = false;

    std::vector<glm::mat4> m_Transformations;

    std::shared_ptr<VulkanVertexBuffer> m_VertexBuffer;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<VulkanVertexBuffer>> m_EntityVertexBuffers;
    std::shared_ptr<VulkanIndexBuffer> m_IndexBuffer;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<VulkanIndexBuffer>> m_EntityIndexBuffers;
    std::shared_ptr<VulkanShaderStorageBuffer<glm::mat4>> m_ModelStorageBuffer;
    std::shared_ptr<VulkanUniformBuffer<Camera>> m_CameraUniformBuffer;
    std::shared_ptr<VulkanUniformBuffer<Light>> m_LightUniformBuffer;

    // TEMPORARY : Hardcoded camera and light data
    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<Light> m_Light;

    public:
    // Constructor that initializes the window and Vulkan objects
    static VulkanRenderer *Create(GLFWwindow *window);
    VulkanRenderer(GLFWwindow *window);
    void createEntityResources();

    // Destructor
    virtual ~VulkanRenderer();

    // Pure virtual functions from Renderer that must be overridden
    virtual void Draw() override;
    virtual void BeginRecord() override;
    virtual void EndRecord() override;
    virtual void SetWindow(GLFWwindow *window) override { m_Window = window; }
    virtual void SetWindowResized(bool resized) override;
    virtual void SetWindowMinimized(bool minimized) override;
    virtual void WaitIdle() override;
    void SetEntityManager(std::shared_ptr<EntityManager> entitymanager) { m_EntityManager = entitymanager; }
    void recreateSwapChain();

    std::shared_ptr<VulkanInstance> GetVulkanInstance() { return m_Instance; }
    std::shared_ptr<VulkanSwapChain> GetSwapChain() { return m_SwapChain; }
    std::shared_ptr<VulkanFrameBuffer> GetFrameBuffer() { return m_FrameBuffer; }
    std::shared_ptr<VulkanDevice> GetDevice() { return m_Device; }
    std::shared_ptr<VulkanRenderPass> GetRenderPass() { return m_RenderPass; }
    std::shared_ptr<VulkanCommandBuffer> GetCommandBuffer() { return m_CommandBuffer; }
    uint32_t GetCurrentFrame() { return m_CurrentFrame; }
    uint32_t GetImageIndex() { return m_ImageIndex; }
    void SetEntityUpdate(bool update) { m_EntityUpdate = update; }
    bool GetEntityUpdate() { return m_EntityUpdate; }

    private:
    // Initializes Vulkan instance and debug messenger
    virtual void Init() override;
};
} // namespace Engine