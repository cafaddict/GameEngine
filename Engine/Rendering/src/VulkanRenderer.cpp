#include "Log.hpp"
#include "ModelComponent.hpp"
#include "ShaderComponent.hpp"
#include "TextureComponent.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanInstance.hpp"
#include "VulkanShader.hpp"
#include <VulkanRenderer.hpp>
#include <_types/_uint32_t.h>
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
    m_CommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().graphicsFamily.value());
    ENGINE_WARN("Vulkan Command Buffer Created");
    m_ComputeCommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().computeFamily.value());
    ENGINE_WARN("Vulkan Compute Command Buffer Created");
    m_TransferCommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().transferFamily.value());
    ENGINE_WARN("Vulkan Transfer Command Buffer Created");
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

void VulkanRenderer_refac::createEntityResources() {

    uint32_t currentVertexOffset = 0;
    uint32_t currentIndexOffset = 0;

    auto entities = m_EntityManager->GetAllEntities();

    for (auto &entity : entities) {
        auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();
        auto texture_data = entity->GetComponent<TextureComponent>()->GetTextureData();
        auto vertex_shader_data = entity->GetComponent<ShaderComponent>()->GetVertexShader();
        auto fragment_shader_data = entity->GetComponent<ShaderComponent>()->GetFragmentShader();
        auto compute_shader_data = entity->GetComponent<ShaderComponent>()->GetComputeShader();

        size_t vertexCount = model_data->positions.size();
        size_t indexCount = model_data->indices.size();

        VulkanShadersData shaders;
        shaders.vertexShader = vertex_shader_data->GetShaderCode();

        shaders.fragShader = fragment_shader_data->GetShaderCode();

        if (compute_shader_data != nullptr) {
            shaders.computeShader = compute_shader_data->GetShaderCode(); // Assign shader code if not nullptr
        } else {
            shaders.computeShader = std::nullopt; // Assign std::nullopt if fragment_shader_data is nullptr
        }

        auto it = m_PipelineCache.find(shaders);

        if (it != m_PipelineCache.end()) {
            m_EntityPipelines[entity] = it->second;
        } else {
            m_EntityPipelines[entity] = VulkanGraphicsPipeline(m_Device, m_RenderPass, shaders);
            m_PipelineCache[shaders] = m_EntityPipelines[entity];
        }
    }
}

} // namespace Engine