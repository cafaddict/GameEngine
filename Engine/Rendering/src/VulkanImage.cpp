#include "VulkanImage.hpp"
#include "vulkan/vulkan_core.h"
namespace Engine {
VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> device, VkImageCreateInfo imageInfo,
                         VkMemoryPropertyFlags property, VkImageAspectFlags aspectFlags)
    : m_Device(device) {
    createImage(imageInfo, property);
    m_ImageView = createImageView(m_Image, imageInfo.format, aspectFlags, imageInfo.mipLevels);
}
VulkanImage::~VulkanImage() {
    vkDestroyImageView(m_Device->getLogicalDevice(), m_ImageView, nullptr);
    vkDestroyImage(m_Device->getLogicalDevice(), m_Image, nullptr);
    vkFreeMemory(m_Device->getLogicalDevice(), m_ImageMemory, nullptr);
}
void VulkanImage::createImage(VkImageCreateInfo imageInfo, VkMemoryPropertyFlags property) {

    if (vkCreateImage(m_Device->getLogicalDevice(), &imageInfo, nullptr, &m_Image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image!");
    }
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device->getLogicalDevice(), m_Image, &memRequirements);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device->findMemoryType(memRequirements.memoryTypeBits, property);
    if (vkAllocateMemory(m_Device->getLogicalDevice(), &allocInfo, nullptr, &m_ImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate image memory!");
    }
    vkBindImageMemory(m_Device->getLogicalDevice(), m_Image, m_ImageMemory, 0);
}
VkImageView VulkanImage::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                         uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    VkImageView imageView;
    if (vkCreateImageView(m_Device->getLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture image view!");
    }
    return imageView;
}

} // namespace Engine