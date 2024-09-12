#pragma once
#include "VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"
#include <_types/_uint32_t.h>
#include <memory>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanCommandBuffer {
    public:
    VulkanCommandBuffer(std::shared_ptr<VulkanDevice> device, uint32_t queueFamiliyValue);
    ~VulkanCommandBuffer();

    VkCommandPool getCommandPool() { return m_CommandPool; }
    std::vector<VkCommandBuffer> getCommandBuffers() { return m_CommandBuffers; }

    private:
    std::shared_ptr<VulkanDevice> m_Device;

    VkCommandPool m_CommandPool;

    std::vector<VkCommandBuffer> m_CommandBuffers;

    void createCommandPool(std::shared_ptr<VulkanDevice> device, uint32_t queueFamiliyValue);

    void createCommandBuffers(uint32_t max_frames_in_flight);
};
} // namespace Engine