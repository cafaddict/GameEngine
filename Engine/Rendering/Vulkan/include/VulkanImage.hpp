#pragma once
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "vulkan/vulkan_core.h"
#include <_types/_uint32_t.h>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanImage {
  public:
    VulkanImage() = default;
    VulkanImage(std::shared_ptr<VulkanDevice> device,
                VkImageCreateInfo imageInfo, VkMemoryPropertyFlags property,
                VkImageAspectFlags aspectFlags);
    VulkanImage(std::shared_ptr<VulkanDevice> device,
                std::shared_ptr<VulkanCommandBuffer> commandBuffer,
                VkImageCreateInfo imageInfo, VkMemoryPropertyFlags property,
                VkImageAspectFlags aspectFlags);
    ~VulkanImage();

    VkImageView getImageView() { return m_ImageView; }
    VkImage getImage() { return m_Image; }
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout,
                               VkFormat format, uint32_t mipLevels);

    void createImage(VkImageCreateInfo imageInfo,
                     VkMemoryPropertyFlags property);
    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspectFlags,
                                uint32_t mipLevels);

  private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanCommandBuffer> m_CommandBuffer;

    VkImage m_Image;
    VkDeviceMemory m_ImageMemory;
    VkImageView m_ImageView;
    VkFormat m_Format;

    void transitionImageLayout(VkImage image, VkFormat format,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                           uint32_t height);
    void createTextureSampler();
};
} // namespace Engine