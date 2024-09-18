#include "VulkanCommandBuffer.hpp"
#include "Log.hpp"
#include "VulkanDevice.hpp"
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
} // namespace Engine