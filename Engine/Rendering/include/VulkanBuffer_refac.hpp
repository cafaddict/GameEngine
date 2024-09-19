#pragma once
#include "VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vulkan/vulkan.h>
#include <variant>
namespace Engine {

template <typename T> class VulkanBuffer_refac {
    public:
    VulkanBuffer_refac() = default;
    VulkanBuffer_refac(T data, std::shared_ptr<VulkanDevice> device, VkBufferUsageFlagBits usage);
    ~VulkanBuffer_refac();
    std::variant<VkBuffer, std::vector<VkBuffer>> getBuffer() const;

    private:
    VkBufferUsageFlagBits m_Usage;
    std::vector<VkBuffer> m_Buffers;
    std::vector<VkDeviceMemory> m_BuffersMemory;
    std::vector<void *> m_BuffersMapped;

    T m_Data;
    std::shared_ptr<VulkanDevice> m_Device;
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};
} // namespace Engine
