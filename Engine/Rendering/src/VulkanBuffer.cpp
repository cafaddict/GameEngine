#include "VulkanBuffer.hpp"
#include "Log.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"
namespace Engine {
void VulkanBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                VkBuffer &buffer, VkDeviceMemory &bufferMemory,
                                std::shared_ptr<VulkanDevice> m_Device) {

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    std::vector<uint32_t> queueFamilyIndices = m_Device->getQueueFamilyIndices().getUniqueFamilies();

    bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
    bufferInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;

    if (vkCreateBuffer(m_Device->getLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Device->getLogicalDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, m_Device);

    if (vkAllocateMemory(m_Device->getLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_Device->getLogicalDevice(), buffer, bufferMemory, 0);
}

void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                              std::shared_ptr<VulkanCommandBuffer> m_TransferCommandBuffer) {
    m_TransferCommandBuffer->beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(m_TransferCommandBuffer->getCommandBuffers()[0], srcBuffer, dstBuffer, 1, &copyRegion);
    m_TransferCommandBuffer->endSingleTimeCommands();
};

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,
                                      std::shared_ptr<VulkanDevice> m_Device) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_Device->getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}
VulkanVertexBuffer::VulkanVertexBuffer(std::vector<VulkanVertex> &vertices, std::shared_ptr<VulkanDevice> device,
                                       std::shared_ptr<VulkanCommandBuffer> transferCommandBuffer)
    : m_Device(device), m_TransferCommandBuffer(transferCommandBuffer) {
    m_VertexCount = vertices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory statingBufferMemory;
    VkDeviceSize bufferSize = sizeof(VulkanVertex) * vertices.size();

    m_Buffers.resize(1);
    m_BuffersMemory.resize(1);
    m_BuffersMapped.resize(1);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                 statingBufferMemory, m_Device);
    void *data;
    vkMapMemory(m_Device->getLogicalDevice(), statingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, (void *)vertices.data(), (size_t)bufferSize);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffers[0], m_BuffersMemory[0], m_Device);
    copyBuffer(stagingBuffer, m_Buffers[0], bufferSize, m_TransferCommandBuffer);
    vkDestroyBuffer(m_Device->getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device->getLogicalDevice(), statingBufferMemory, nullptr);
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    for (size_t i = 0; i < m_Buffers.size(); i++) {
        vkDestroyBuffer(m_Device->getLogicalDevice(), m_Buffers[i], nullptr);
        vkFreeMemory(m_Device->getLogicalDevice(), m_BuffersMemory[i], nullptr);
    }
}

VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t> &indices, std::shared_ptr<VulkanDevice> device,
                                     std::shared_ptr<VulkanCommandBuffer> transferCommandBuffer)
    : m_Device(device), m_TransferCommandBuffer(transferCommandBuffer) {
    m_IndexCount = indices.size();
    VkBuffer stagingBuffer;
    VkDeviceMemory statingBufferMemory;
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    m_Buffers.resize(1);
    m_BuffersMemory.resize(1);
    m_BuffersMapped.resize(1);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                 statingBufferMemory, m_Device);
    void *data;
    vkMapMemory(m_Device->getLogicalDevice(), statingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffers[0], m_BuffersMemory[0], m_Device);
    copyBuffer(stagingBuffer, m_Buffers[0], bufferSize, m_TransferCommandBuffer);
    vkDestroyBuffer(m_Device->getLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(m_Device->getLogicalDevice(), statingBufferMemory, nullptr);
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
    for (size_t i = 0; i < m_Buffers.size(); i++) {
        vkDestroyBuffer(m_Device->getLogicalDevice(), m_Buffers[i], nullptr);
        vkFreeMemory(m_Device->getLogicalDevice(), m_BuffersMemory[i], nullptr);
    }
}

} // namespace Engine
