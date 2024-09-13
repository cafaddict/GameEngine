#include "Log.hpp"
#include "ModelComponent.hpp"
#include "ShaderComponent.hpp"
#include "TextureComponent.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanInstance.hpp"
#include "VulkanShader.hpp"
#include <VulkanRenderer.hpp>
#include "VulkanVertex.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#define MAX_FRAMES_IN_FLIGHT 2
namespace Engine {

Renderer *Renderer::Create(GLFWwindow *window) {
    ENGINE_INFO("Vulkan Renderer Creation");
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
    ENGINE_INFO("Vulkan Renderer Init");
    m_Instance = std::make_shared<VulkanInstance>();
    ENGINE_INFO("Vulkan Instance Created");
    m_DebugMessenger = std::make_shared<VulkanDebugMessenger>(m_Instance);
    ENGINE_INFO("Vulkan Debug Messenger Created");
    m_Device = std::make_shared<VulkanDevice>(m_Instance, m_Window);
    ENGINE_INFO("Vulkan Device Created");
    m_SwapChain = std::make_shared<VulkanSwapChain>(m_Device, m_Window);
    ENGINE_INFO("Vulkan Swap Chain Created");
    m_RenderPass = std::make_shared<VulkanRenderPass>(m_Device, m_SwapChain);
    ENGINE_INFO("Vulkan Render Pass Created");
    m_FrameBuffer = std::make_shared<VulkanFrameBuffer>(m_Device, m_SwapChain, m_RenderPass);
    ENGINE_INFO("Vulkan Frame Buffer Created");
    m_CommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().graphicsFamily.value());
    ENGINE_INFO("Vulkan Command Buffer Created");
    m_ComputeCommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().computeFamily.value());
    ENGINE_INFO("Vulkan Compute Command Buffer Created");
    m_TransferCommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().transferFamily.value());
    ENGINE_INFO("Vulkan Transfer Command Buffer Created");
    m_ImageAvailableSemaphore = std::make_shared<VulkanSemaphore>(m_Device, MAX_FRAMES_IN_FLIGHT);
    ENGINE_INFO("Vulkan Image Available Semaphore Created");
    m_RenderFinishedSemaphore = std::make_shared<VulkanSemaphore>(m_Device, MAX_FRAMES_IN_FLIGHT);
    ENGINE_INFO("Vulkan Render Finished Semaphore Created");
    m_ComputeFinishedSemaphore = std::make_shared<VulkanSemaphore>(m_Device, MAX_FRAMES_IN_FLIGHT);
    ENGINE_INFO("Vulkan Compute Finished Semaphore Created");
    m_TransferFinishedSemaphore = std::make_shared<VulkanSemaphore>(m_Device, MAX_FRAMES_IN_FLIGHT);
    ENGINE_INFO("Vulkan Transfer Finished Semaphore Created");
    m_InFlightFences = std::make_shared<VulkanFence>(m_Device, MAX_FRAMES_IN_FLIGHT);
    ENGINE_INFO("Vulkan In Flight Fences Created");
    m_ComputeFences = std::make_shared<VulkanFence>(m_Device, MAX_FRAMES_IN_FLIGHT);
    ENGINE_INFO("Vulkan Compute Fences Created");
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
void VulkanRenderer_refac::Draw() {}
void VulkanRenderer_refac::BeginRecord() {
    vkWaitForFences(m_Device->getLogicalDevice(), 1, &m_InFlightFences->getFence()[m_CurrentFrame], VK_TRUE,
                    UINT64_MAX);
    VkResult result =
        vkAcquireNextImageKHR(m_Device->getLogicalDevice(), m_SwapChain->getSwapChain(), UINT64_MAX,
                              m_ImageAvailableSemaphore->getSemaphore()[m_CurrentFrame], VK_NULL_HANDLE, &m_ImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(m_Device->getLogicalDevice(), 1, &m_InFlightFences->getFence()[m_CurrentFrame]);
    vkResetCommandBuffer(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], 1);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass->getRenderPass();
    renderPassInfo.framebuffer = m_FrameBuffer->getFrameBuffers()[m_ImageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
}
void VulkanRenderer_refac::EndRecord() {
    if (vkEndCommandBuffer(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphore->getSemaphore()[m_CurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer->getCommandBuffers()[m_CurrentFrame];
    VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphore->getSemaphore()[m_CurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_Device->getGraphicsQueue(), 1, &submitInfo, m_InFlightFences->getFence()[m_CurrentFrame]) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {m_SwapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_ImageIndex;
    presentInfo.pResults = nullptr; // Optional
    VkResult result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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
        shaders.vertexShader =
            vertex_shader_data ? std::make_optional(vertex_shader_data->GetShaderCode()) : std::nullopt;

        shaders.fragShader =
            fragment_shader_data ? std::make_optional(fragment_shader_data->GetShaderCode()) : std::nullopt;

        shaders.computeShader =
            compute_shader_data ? std::make_optional(compute_shader_data->GetShaderCode()) : std::nullopt;

        auto it = m_PipelineCache.find(shaders);

        if (it != m_PipelineCache.end()) {
            m_EntityPipelines[entity] = it->second;
        } else {
            VulkanVertex vertex;
            m_EntityPipelines[entity] = VulkanGraphicsPipeline(m_Device, m_RenderPass, shaders, vertex);
            m_PipelineCache[shaders] = m_EntityPipelines[entity];
        }
    }
}

} // namespace Engine