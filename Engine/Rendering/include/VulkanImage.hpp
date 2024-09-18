#pragma once
#include <vulkan/vulkan.h>
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "vulkan/vulkan_core.h"
namespace Engine {
class VulkanImage {
    public:
    VulkanImage() = default;
    VulkanImage(std::shared_ptr<VulkanDevice> device, VkImageCreateInfo imageInfo, VkMemoryPropertyFlags property,
                VkImageAspectFlags aspectFlags);
    ~VulkanImage();

    VkImageView getImageView() { return m_ImageView; }
    VkImage getImage() { return m_Image; }

    private:
    std::shared_ptr<VulkanDevice> m_Device;

    VkImage m_Image;
    VkDeviceMemory m_ImageMemory;
    VkImageView m_ImageView;
    VkFormat m_Format;

    void createImage(VkImageCreateInfo imageInfo, VkMemoryPropertyFlags property);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createTextureSampler();
};
} // namespace Engine