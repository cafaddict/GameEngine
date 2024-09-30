#pragma once
#include "Entity.hpp"
#include "Log.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "glm/fwd.hpp"
#include "glm/mat4x4.hpp" // Include the header for glm::mat4
#include "vulkan/vulkan_core.h"
#include <_types/_uint32_t.h>
#include <cstddef>
#include <iostream>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vulkan/vulkan.h>
#define MAX_FRAME_IN_FLIGHT 2
template <typename T> struct is_vector : std::false_type {};

template <typename T, typename Alloc> struct is_vector<std::vector<T, Alloc>> : std::true_type {};

namespace Engine {

// Type trait to check if T is a std::vector

template <typename T> class VulkanBuffer_refac {
    public:
    VulkanBuffer_refac() = default;
    VulkanBuffer_refac(T &data, std::unordered_map<std::shared_ptr<Entity>, size_t> entityOffsets,
                       std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanCommandBuffer> transferCommandBuffer,
                       VkBufferUsageFlagBits usage);
    ~VulkanBuffer_refac();
    std::variant<VkBuffer, std::vector<VkBuffer>> getBuffer() const;
    std::unordered_map<std::shared_ptr<Entity>, size_t> getOffsets() const { return m_EntityOffsets; }
    size_t getDataSize() const { return m_DataSize; }
    T &getData() { return m_Data; }
    void updateData(T &data, uint32_t currentFrame) {
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
            return;
        }
        memcpy(m_BuffersMapped[currentFrame], data.data(), sizeof(T) * data.size());
    }

    private:
    VkBufferUsageFlagBits m_Usage;
    std::vector<VkBuffer> m_Buffers;
    std::vector<VkDeviceMemory> m_BuffersMemory;
    std::vector<void *> m_BuffersMapped;

    T m_Data;
    size_t m_DataSize;
    std::unordered_map<std::shared_ptr<Entity>, size_t> m_EntityOffsets;
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanCommandBuffer> m_TransferCommandBuffer;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_Device->getLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(m_Device->getLogicalDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device->getLogicalDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(m_Device->getLogicalDevice(), buffer, bufferMemory, 0);
    }
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        m_TransferCommandBuffer->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(m_TransferCommandBuffer->getCommandBuffers()[0], srcBuffer, dstBuffer, 1, &copyRegion);
        m_TransferCommandBuffer->endSingleTimeCommands();
    };
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_Device->getPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
};

template <typename T> std::variant<VkBuffer, std::vector<VkBuffer>> VulkanBuffer_refac<T>::getBuffer() const {
    if (m_Usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT || m_Usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        return m_Buffers;
    } else {
        return m_Buffers[0];
    }
}

template <typename T>
VulkanBuffer_refac<T>::VulkanBuffer_refac(T &data, std::unordered_map<std::shared_ptr<Entity>, size_t> entityOffsets,
                                          std::shared_ptr<VulkanDevice> device,
                                          std::shared_ptr<VulkanCommandBuffer> transferCommandBuffer,
                                          VkBufferUsageFlagBits usage)
    : m_Data(data), m_EntityOffsets(entityOffsets), m_Device(device), m_TransferCommandBuffer(transferCommandBuffer),
      m_Usage(usage) {
    bool T_is_vecotr = false;
    if constexpr (is_vector<T>::value) {
        T_is_vecotr = true;
    }
    // if ((m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || m_Usage ==
    // VK_BUFFER_USAGE_INDEX_BUFFER_BIT) && T_is_vecotr)
    // {
    if constexpr (is_vector<T>::value) {
        if (m_Usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
            using ElementType = typename T::value_type;
            VkDeviceSize bufferSize = sizeof(ElementType) * 1000;
            m_DataSize = sizeof(ElementType);
            m_Buffers.resize(MAX_FRAME_IN_FLIGHT);
            m_BuffersMemory.resize(MAX_FRAME_IN_FLIGHT);
            m_BuffersMapped.resize(MAX_FRAME_IN_FLIGHT);
            for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
                createBuffer(bufferSize, m_Usage,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers[i],
                             m_BuffersMemory[i]);
                vkMapMemory(m_Device->getLogicalDevice(), m_BuffersMemory[i], 0, bufferSize, 0, &m_BuffersMapped[i]);
            }
            return;
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory statingBufferMemory;
        using ElementType = typename T::value_type;
        VkDeviceSize bufferSize = sizeof(ElementType) * m_Data.size();
        m_DataSize = sizeof(ElementType);
        m_Buffers.resize(1);
        m_BuffersMemory.resize(1);
        m_BuffersMapped.resize(1);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                     statingBufferMemory);
        void *data;
        vkMapMemory(m_Device->getLogicalDevice(), statingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_Data.data(), (size_t)bufferSize);
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | m_Usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     m_Buffers[0], m_BuffersMemory[0]);
        copyBuffer(stagingBuffer, m_Buffers[0], bufferSize);
        vkDestroyBuffer(m_Device->getLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device->getLogicalDevice(), statingBufferMemory, nullptr);
    } else {
        VkDeviceSize bufferSize = sizeof(T);
        if (m_Usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
            bufferSize = sizeof(T) * 1000;
        }
        m_DataSize = sizeof(T);
        m_Buffers.resize(MAX_FRAME_IN_FLIGHT);
        m_BuffersMemory.resize(MAX_FRAME_IN_FLIGHT);
        m_BuffersMapped.resize(MAX_FRAME_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
            createBuffer(bufferSize, m_Usage,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers[i],
                         m_BuffersMemory[i]);
            vkMapMemory(m_Device->getLogicalDevice(), m_BuffersMemory[i], 0, bufferSize, 0, &m_BuffersMapped[i]);
        }
    }
}
template <typename T> VulkanBuffer_refac<T>::~VulkanBuffer_refac() {
    for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
        vkDestroyBuffer(m_Device->getLogicalDevice(), m_Buffers[i], nullptr);
        vkFreeMemory(m_Device->getLogicalDevice(), m_BuffersMemory[i], nullptr);
        if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
            return;
        }
    }
}
} // namespace Engine
