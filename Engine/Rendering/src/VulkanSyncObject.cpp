#include "VulkanSyncObject.hpp"
#include "VulkanDevice.hpp"
#include <memory>
#include "Log.hpp"
namespace Engine {
VulkanSemaphore::VulkanSemaphore(std::shared_ptr<VulkanDevice> device, uint32_t max_frames_in_flight)
    : m_Device(device) {
    m_Semaphores.resize(max_frames_in_flight);
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (uint32_t i = 0; i < max_frames_in_flight; i++) {
        if (vkCreateSemaphore(m_Device->getLogicalDevice(), &semaphoreInfo, nullptr, &m_Semaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }
}
VulkanSemaphore::~VulkanSemaphore() {
    ENGINE_INFO("VulkanSemaphore is destroyed");
    for (auto semaphore : m_Semaphores) {
        vkDestroySemaphore(m_Device->getLogicalDevice(), semaphore, nullptr);
    }
}
VulkanFence::VulkanFence(std::shared_ptr<VulkanDevice> device, uint32_t max_frames_in_flight) : m_Device(device) {
    m_Fences.resize(max_frames_in_flight);
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (uint32_t i = 0; i < max_frames_in_flight; i++) {
        if (vkCreateFence(m_Device->getLogicalDevice(), &fenceInfo, nullptr, &m_Fences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create fences!");
        }
    }
}
VulkanFence::~VulkanFence() {
    ENGINE_INFO("VulkanFence is destroyed");
    for (auto fence : m_Fences) {
        vkDestroyFence(m_Device->getLogicalDevice(), fence, nullptr);
    }
}
} // namespace Engine