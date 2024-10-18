#include "VulkanImage.hpp"
#include "Log.hpp"
#include "VulkanCommandBuffer.hpp"
namespace Engine {
VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> device, VkImageCreateInfo imageInfo,
                         VkMemoryPropertyFlags property, VkImageAspectFlags aspectFlags)
    : m_Device(device) {
    createImage(imageInfo, property);
    createImageView(m_Image, imageInfo.format, aspectFlags, imageInfo.mipLevels);
}

VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanCommandBuffer> commandBuffer,
                         VkImageCreateInfo imageInfo, VkMemoryPropertyFlags property, VkImageAspectFlags aspectFlags)
    : m_Device(device), m_CommandBuffer(commandBuffer) {
    createImage(imageInfo, property);
    m_Format = imageInfo.format;
    ENGINE_WARN("Vulkan Image Created");
    createImageView(m_Image, imageInfo.format, aspectFlags, imageInfo.mipLevels);
}
VulkanImage::~VulkanImage() {
    ENGINE_INFO("Vulkan Image Destroyted");
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
    if (vkBindImageMemory(m_Device->getLogicalDevice(), m_Image, m_ImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to bind image memory!");
    }
}

void VulkanImage::createImageView() {
    ENGINE_WARN("Creating Image View");

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_Format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device->getLogicalDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view!");
    }
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
    if (vkCreateImageView(m_Device->getLogicalDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view!");
    }

    return imageView;
}
bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
void VulkanImage::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkFormat format,
                                        uint32_t mipLevels) {

    m_CommandBuffer->beginSingleTimeCommands();
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = m_Image;
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
               newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(m_CommandBuffer->getCommandBuffers()[0], sourceStage, destinationStage, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);
    m_CommandBuffer->endSingleTimeCommands();
}

} // namespace Engine