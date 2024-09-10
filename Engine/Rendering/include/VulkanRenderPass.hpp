#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
namespace Engine {
class VulkanRenderPass {
    public:
    VulkanRenderPass(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapChain);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass() { return m_RenderPass; }

    private:
    void createRenderPass();
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanSwapChain> m_SwapChain;
    VkRenderPass m_RenderPass;
};
} // namespace Engine