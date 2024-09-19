#include "VulkanBuffer_refac.hpp"
#include "VulkanBuffer.hpp"
#include "vulkan/vulkan_core.h"
#define MAX_FRAME_IN_FLIGHT 2
namespace Engine {

template <typename T> std::variant<VkBuffer, std::vector<VkBuffer>> VulkanBuffer_refac<T>::getBuffer() const {
    if (m_Usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT || m_Usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        return m_Buffers[0];
    } else {
        return m_Buffers;
    }
}

template <typename T>
VulkanBuffer_refac<T>::VulkanBuffer_refac(T data, std::shared_ptr<VulkanDevice> device, VkBufferUsageFlagBits usage)
    : m_Data(data), m_Device(device), m_Usage(usage) {
    if (m_Usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || m_Usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        auto a = T::value_type;
        VkDeviceSize bufferSize = sizeoof(T::value_type) * m_Data.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory statingBufferMemory;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                     m_BuffersMemory[0]);
        void *data;
        vkMapMemory(m_Device->getLogicalDevice(), statingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_Data.data(), (size_t)bufferSize);
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | m_Usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     m_Buffers[0], m_BuffersMemory[0]);
        copyBuffer(stagingBuffer, m_Buffers[0], bufferSize);
        vkDestroyBuffer(m_Device->getLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device->getLogicalDevice(), statingBufferMemory, nullptr);
    } else {
        m_Buffers.resize(MAX_FRAME_IN_FLIGHT);
        m_BuffersMemory.resize(MAX_FRAME_IN_FLIGHT);
        m_BuffersMapped.resize(MAX_FRAME_IN_FLIGHT);
        for (int i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
            VkDeviceSize bufferSize = sizeof(T);
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