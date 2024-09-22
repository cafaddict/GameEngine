#pragma once
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <memory>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanFrameBuffer {
  public:
    VulkanFrameBuffer(std::shared_ptr<VulkanDevice> device,
                      std::shared_ptr<VulkanSwapChain> swapChain,
                      std::shared_ptr<VulkanRenderPass> renderPass,
                      std::shared_ptr<VulkanCommandBuffer> commandBuffer);
    ~VulkanFrameBuffer();

    std::vector<VkFramebuffer> getFrameBuffers() { return m_FrameBuffers; }

  private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanSwapChain> m_SwapChain;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;
    std::shared_ptr<VulkanCommandBuffer> m_CommandBuffer;

    std::vector<VkFramebuffer> m_FrameBuffers;
    VkExtent2D m_Extent;
    std::shared_ptr<VulkanImage> m_ColorImage;
    std::shared_ptr<VulkanImage> m_DepthImage;

    void createFrameBuffer();
};
} // namespace Engine