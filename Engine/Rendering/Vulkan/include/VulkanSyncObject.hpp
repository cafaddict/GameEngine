#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanDevice.hpp"
namespace Engine {
class VulkanSemaphore {
    public:
    VulkanSemaphore(std::shared_ptr<VulkanDevice> device, uint32_t max_frames_in_flight);
    ~VulkanSemaphore();
    std::vector<VkSemaphore> getSemaphore() { return m_Semaphores; }

    private:
    std::vector<VkSemaphore> m_Semaphores;
    std::shared_ptr<VulkanDevice> m_Device;
};
class VulkanFence {
    public:
    VulkanFence(std::shared_ptr<VulkanDevice> device, uint32_t max_frames_in_flight);
    ~VulkanFence();
    std::vector<VkFence> getFence() { return m_Fences; }

    private:
    std::vector<VkFence> m_Fences;
    std::shared_ptr<VulkanDevice> m_Device;
};
} // namespace Engine