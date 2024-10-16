#include "Camera.hpp"
#include "Light.hpp"
#include "Log.hpp"
#include "ModelComponent.hpp"
#include "ShaderComponent.hpp"
#include "TextureComponent.hpp"
#include "TransformComponent.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanDescriptorSet.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanInstance.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanVertex.hpp"
#include "glm/fwd.hpp"
#include "vulkan/vulkan_core.h"
#include <VulkanRenderer.hpp>
#include <cstddef>
#include <memory>
#include <unordered_map>

#define MAX_FRAMES_IN_FLIGHT 2
namespace Engine {

// Renderer *Renderer::Create(GLFWwindow *window) {
//     ENGINE_INFO("Vulkan Renderer Creation");
//     return new VulkanRenderer(window);
// }

// Constructor implementation
VulkanRenderer::VulkanRenderer(GLFWwindow *window) {
    SetWindow(window);
    Init(); // Initialize Vulkan instance and debug messenger
}

VulkanRenderer *VulkanRenderer::Create(GLFWwindow *window) { return new VulkanRenderer(window); }

// Destructor implementation
VulkanRenderer::~VulkanRenderer() {
    // Vulkan cleanup (if needed) happens automatically due to shared_ptr
    // destructors
}

// Init implementation to set up Vulkan instance and debug messenger
void VulkanRenderer::Init() {
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

    m_CommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().graphicsFamily.value());
    ENGINE_INFO("Vulkan Command Buffer Created");

    m_ComputeCommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().computeFamily.value());
    ENGINE_INFO("Vulkan Compute Command Buffer Created");

    m_TransferCommandBuffer =
        std::make_shared<VulkanCommandBuffer>(m_Device, m_Device->getQueueFamilyIndices().transferFamily.value());
    ENGINE_INFO("Vulkan Transfer Command Buffer Created");

    m_FrameBuffer = std::make_shared<VulkanFrameBuffer>(m_Device, m_SwapChain, m_RenderPass, m_TransferCommandBuffer);
    ENGINE_INFO("Vulkan Frame Buffer Created");

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

    // TEMPORARY : Hardcoded camera and light data
    m_Camera = std::make_shared<Camera>(
        glm::lookAt(glm::vec3(0.0f, 450.0f, 350.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::perspective(glm::radians(45.0f),
                         (float)m_SwapChain->getSwapChainExtent().width /
                             (float)m_SwapChain->getSwapChainExtent().height,
                         0.01f, 1000.0f),
        glm::vec3(0.0f, 450.0f, 350.0f));

    m_Light = std::make_shared<Light>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);

    std::unordered_map<std::shared_ptr<Entity>, size_t> offsets;

    m_CameraUniformBuffer = std::make_shared<VulkanUniformBuffer<Camera>>(m_Camera, m_Device, m_TransferCommandBuffer);
    ENGINE_INFO("Vulkan Camera Uniform Buffer Created");
    m_LightUniformBuffer = std::make_shared<VulkanUniformBuffer<Light>>(m_Light, m_Device, m_TransferCommandBuffer);
    ENGINE_INFO("Vulkan Light Uniform Buffer Created");
    std::vector<glm::mat4> transformations;
    m_ModelStorageBuffer =
        std::make_shared<VulkanShaderStorageBuffer<glm::mat4>>(transformations, m_Device, m_TransferCommandBuffer);
    ENGINE_INFO("Vulkan Model Storage Buffer Created");
    // createEntityResources();
}

// Implement pure virtual function from Renderer
void VulkanRenderer::SetWindowResized(bool resized) {
    // Handle window resize
    m_Resized = resized;
}

// Implement pure virtual function from Renderer
void VulkanRenderer::SetWindowMinimized(bool minimized) {
    // Handle window minimized logic
    m_Minimizied = minimized;
}

// Implement pure virtual function from Renderer
void VulkanRenderer::WaitIdle() {
    // Implement Vulkan logic to wait for GPU operations to finish
}

// Implement pure virtual function from Renderer
void VulkanRenderer::Draw() {

    auto entities = m_EntityManager->GetAllEntities();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);

    viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_SwapChain->getSwapChainExtent();
    vkCmdSetScissor(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], 0, 1, &scissor);
    VkDeviceSize offsets[] = {0};

    if (!entities.empty() && !m_EntityUpdate) {
        // VkBuffer vertexBuffers[] = {m_VertexBuffer->getBuffer()[0]};

        // VkBuffer indexBuffer = m_IndexBuffer->getBuffer()[0];

        // vkCmdBindVertexBuffers(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], 0, 1, vertexBuffers, offsets);
        // vkCmdBindIndexBuffer(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], indexBuffer, 0,
        //                      VK_INDEX_TYPE_UINT32);
        int cnt = 0;
        std::vector<glm::mat4> transformations;
        for (const auto &entity : entities) {
            auto transform = entity->GetComponent<TransformComponent>()->GetTransformMatrix();
            transformations.push_back(transform);
        }
        for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
            m_ModelStorageBuffer->updateData(transformations, i);
        }

        for (const auto &entity : entities) {
            auto VulkanVertexBuffer = m_EntityVertexBuffers[entity];
            auto VulkanIndexBuffer = m_EntityIndexBuffers[entity];
            VkBuffer vertexBuffers[] = {VulkanVertexBuffer->getBuffer()[0]};
            VkBuffer indexBuffer = VulkanIndexBuffer->getBuffer()[0];

            vkCmdBindVertexBuffers(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], indexBuffer, 0,
                                 VK_INDEX_TYPE_UINT32);

            auto graphicsPipeline = m_EntityPipelines[entity];
            auto descriptorSet = m_EntityDescriptorSets[entity];
            vkCmdBindPipeline(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                              graphicsPipeline->getGraphicsPipeline());

            // VkDeviceSize vertexOffset = m_VertexBuffer->getOffsets()[entity] * m_VertexBuffer->getDataSize();
            // VkDeviceSize indexOffset = m_IndexBuffer->getOffsets()[entity] * m_IndexBuffer->getDataSize();
            VkDeviceSize vertexOffset = VulkanVertexBuffer->getOffsets()[entity];
            VkDeviceSize indexOffset = VulkanIndexBuffer->getOffsets()[entity];

            vkCmdBindDescriptorSets(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame],
                                    VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getPipelineLayout(), 0, 1,
                                    &descriptorSet->getDescriptorSets()[m_CurrentFrame], 0, nullptr);

            vkCmdDrawIndexed(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame],
                             entity->GetComponent<ModelComponent>()->GetModelData()->indices.size(), 1, 0, 0, 0);
            cnt++;
        }
    }
}

void VulkanRenderer::BeginRecord() {

    vkWaitForFences(m_Device->getLogicalDevice(), 1, &m_InFlightFences->getFence()[m_CurrentFrame], VK_TRUE,
                    UINT64_MAX);

    VkResult result =
        vkAcquireNextImageKHR(m_Device->getLogicalDevice(), m_SwapChain->getSwapChain(), UINT64_MAX,
                              m_ImageAvailableSemaphore->getSemaphore()[m_CurrentFrame], VK_NULL_HANDLE, &m_ImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    vkResetFences(m_Device->getLogicalDevice(), 1, &m_InFlightFences->getFence()[m_CurrentFrame]);
    vkResetCommandBuffer(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], 0);

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
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
}
void VulkanRenderer::EndRecord() {
    vkCmdEndRenderPass(m_CommandBuffer->getCommandBuffers()[m_CurrentFrame]);
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
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {m_SwapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_ImageIndex;
    presentInfo.pResults = nullptr; // Optional
    VkResult result = vkQueuePresentKHR(m_Device->getPresentQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Resized) {
        m_Resized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image!");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    if (m_EntityUpdate) {
        createEntityResources();
        m_EntityUpdate = false;
    }
}

void VulkanRenderer::recreateSwapChain() {
    while (m_Minimizied) {
        ENGINE_WARN("Window is minimized. Waiting for restore...");
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_Device->getLogicalDevice());
    m_FrameBuffer.reset();
    // m_RenderPass.reset();
    m_SwapChain->destroy();
    m_SwapChain.reset();

    m_SwapChain = std::make_shared<VulkanSwapChain>(m_Device, m_Window);
    ENGINE_INFO("Vulkan Swap Chain ReCreated");

    // m_RenderPass = std::make_shared<VulkanRenderPass>(m_Device, m_SwapChain);
    // ENGINE_INFO("Vulkan Render Pass ReCreated");

    m_FrameBuffer = std::make_shared<VulkanFrameBuffer>(m_Device, m_SwapChain, m_RenderPass, m_TransferCommandBuffer);
    ENGINE_INFO("Vulkan Frame Buffer ReCreated");
}

void VulkanRenderer::createEntityResources() {
    ENGINE_WARN("Creating Entity Resources");
    uint32_t currentVertexOffset = 0;
    uint32_t currentIndexOffset = 0;

    auto entities = m_EntityManager->GetAllEntities();

    std::vector<glm::mat4> transformations;

    for (auto &entity : entities) {
        std::vector<VulkanVertex> vertices;
        std::vector<uint32_t> indices;
        std::unordered_map<std::shared_ptr<Entity>, size_t> vertexOffsets;
        std::unordered_map<std::shared_ptr<Entity>, size_t> indexOffsets;
        auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();

        auto textureComponent = entity->GetComponents<TextureComponent>();
        std::vector<std::shared_ptr<const TextureData>> texture_data;
        for (auto &texture : textureComponent) {
            texture_data.push_back(texture->GetTextureData());
        }
        // auto texture_data = entity->GetComponent<TextureComponent>()->GetTextureData();

        auto vertex_shader_data = entity->GetComponent<ShaderComponent>()->GetVertexShader();

        auto fragment_shader_data = entity->GetComponent<ShaderComponent>()->GetFragmentShader();

        auto compute_shader_data = entity->GetComponent<ShaderComponent>()->GetComputeShader();

        auto transform = entity->GetComponent<TransformComponent>()->GetTransformMatrix();

        size_t vertexCount = model_data->positions.size();
        size_t indexCount = model_data->indices.size();

        ENGINE_TRACE("model_data->positions.size() = {0}", model_data->positions.size());
        ENGINE_TRACE("model_data->normals.size() = {0}", model_data->normals.size());
        ENGINE_TRACE("model_data->uvs.size() = {0}", model_data->uvs.size());

        VulkanShadersData shaders;
        shaders.vertexShader =
            vertex_shader_data ? std::make_optional(vertex_shader_data->GetShaderCode()) : std::nullopt;
        if (!vertex_shader_data) {
            ENGINE_WARN("Vertex Shader Data is null");
        }

        shaders.fragShader =
            fragment_shader_data ? std::make_optional(fragment_shader_data->GetShaderCode()) : std::nullopt;
        if (!fragment_shader_data) {
            ENGINE_WARN("Fragment Shader Data is null");
        }

        shaders.computeShader =
            compute_shader_data ? std::make_optional(compute_shader_data->GetShaderCode()) : std::nullopt;
        if (!compute_shader_data) {
            ENGINE_WARN("Compute Shader Data is null");
        }

        auto it = m_PipelineCache.find(shaders);

        if (it != m_PipelineCache.end()) {
            m_EntityPipelines[entity] = it->second; // Assign shared pointer to entity pipeline
        } else {
            VulkanVertex vertex;
            // Create a shared pointer for the new pipeline
            auto pipeline =
                std::make_shared<VulkanGraphicsPipeline>(m_Device, m_RenderPass, shaders, vertex, texture_data.size());

            // Store the pipeline in both maps
            m_EntityPipelines[entity] = pipeline;
            m_PipelineCache[shaders] = pipeline;
        }

        for (size_t i = 0; i < vertexCount; i++) {
            VulkanVertex vertex;
            vertex.pos = model_data->positions[i];
            vertex.normal = model_data->normals.size() > i ? model_data->normals[i] : glm::vec3(0.0f, 0.0f, 1.0f);
            vertex.texCoord = model_data->uvs.size() > i ? model_data->uvs[i] : glm::vec2(0.0f);
            vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
            vertices.push_back(vertex);
        }
        indices.insert(indices.end(), model_data->indices.begin(), model_data->indices.end());
        transformations.push_back(transform);
        vertexOffsets[entity] = currentVertexOffset;
        indexOffsets[entity] = currentIndexOffset;
        currentVertexOffset += vertexCount;
        currentIndexOffset += indexCount;

        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos;
        std::vector<VkDescriptorBufferInfo> bufferInfo;

        for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
            std::vector<VkDescriptorBufferInfo> bufferInfo;
            VkDescriptorBufferInfo cameraBufferInfo{};
            cameraBufferInfo.buffer = m_CameraUniformBuffer->getBuffer()[i];
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = m_CameraUniformBuffer->getDataSize();

            VkDescriptorBufferInfo lightBufferInfo{};
            lightBufferInfo.buffer = m_LightUniformBuffer->getBuffer()[i];
            lightBufferInfo.offset = 0;
            lightBufferInfo.range = m_LightUniformBuffer->getDataSize();

            VkDescriptorBufferInfo modelBufferInfo{};
            modelBufferInfo.buffer = m_ModelStorageBuffer->getBuffer()[i];
            modelBufferInfo.offset = 0;
            modelBufferInfo.range = VK_WHOLE_SIZE;

            bufferInfo.push_back(cameraBufferInfo);
            bufferInfo.push_back(lightBufferInfo);
            bufferInfo.push_back(modelBufferInfo);

            bufferInfos.push_back(bufferInfo);
        }

        if (m_EntityDescriptorSets.find(entity) == m_EntityDescriptorSets.end()) {
            auto descriptorset = std::make_shared<VulkanDescriptorSet>(
                m_Device, m_EntityPipelines[entity], m_TransferCommandBuffer, texture_data, bufferInfos);

            m_EntityDescriptorSets[entity] = descriptorset;
        }
        auto vertexBuffer = std::make_shared<VulkanVertexBuffer>(vertices, m_Device, m_TransferCommandBuffer);
        auto indexBuffer = std::make_shared<VulkanIndexBuffer>(indices, m_Device, m_TransferCommandBuffer);

        m_EntityVertexBuffers[entity] = vertexBuffer;
        m_EntityIndexBuffers[entity] = indexBuffer;
    }

    m_Transformations = transformations;

    // m_VertexBuffer = std::make_shared<VulkanVertexBuffer>(vertices, m_Device, m_TransferCommandBuffer);
    // m_IndexBuffer = std::make_shared<VulkanIndexBuffer>(indices, m_Device, m_TransferCommandBuffer);
}

} // namespace Engine
