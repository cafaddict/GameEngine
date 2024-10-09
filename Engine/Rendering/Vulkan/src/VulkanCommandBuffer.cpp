#include "VulkanCommandBuffer.hpp"
#include "Log.hpp"
#include "VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"
#include <_types/_uint32_t.h>
#include <iostream>

#define MAX_FRAMES_IN_FLIGHT 2
namespace Engine {
VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<VulkanDevice> device,
                                         uint32_t queueFamiliyValue)
    : m_Device(device) {
    createCommandPool(m_Device, queueFamiliyValue);

    createCommandBuffers(MAX_FRAMES_IN_FLIGHT);
}
VulkanCommandBuffer::~VulkanCommandBuffer() {
    ENGINE_TRACE("VulkanCommandBuffer destroyed");
    vkDestroyCommandPool(m_Device->getLogicalDevice(), m_CommandPool, nullptr);
    vkFreeCommandBuffers(m_Device->getLogicalDevice(), m_CommandPool,
                         static_cast<uint32_t>(m_CommandBuffers.size()),
                         m_CommandBuffers.data());
}
void VulkanCommandBuffer::createCommandPool(
    std::shared_ptr<VulkanDevice> device, uint32_t queueFamiliyValue) {
    queueFamilyIndices queueFamilyIndices = device->getQueueFamilyIndices();
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamiliyValue;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(m_Device->getLogicalDevice(), &poolInfo, nullptr,
                            &m_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void VulkanCommandBuffer::createCommandBuffers(uint32_t max_frames_in_flight) {
    VkCommandBufferAllocateInfo allocInfo{};
    m_CommandBuffers.resize(max_frames_in_flight);
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(m_CommandBuffers.size());

    if (vkAllocateCommandBuffers(m_Device->getLogicalDevice(), &allocInfo,
                                 m_CommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VulkanCommandBuffer::beginSingleTimeCommands() {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(m_CommandBuffers[0], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
}
void VulkanCommandBuffer::endSingleTimeCommands() {
    if (vkEndCommandBuffer(m_CommandBuffers[0]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffers[0];

    if (vkQueueSubmit(m_Device->getTransferQueue(), 1, &submitInfo,
                      VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    vkQueueWaitIdle(m_Device->getTransferQueue());
    vkResetCommandBuffer(m_CommandBuffers[0], 0);
}
} // namespace Engine