#include "VulkanFrameBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
namespace Engine {

VulkanFrameBuffer::VulkanFrameBuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapChain,
                                     std::shared_ptr<VulkanRenderPass> renderPass)
    : m_Device(device), m_SwapChain(swapChain), m_RenderPass(renderPass) {
    createFrameBuffer();
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
    for (auto framebuffer : m_FrameBuffers) {
        vkDestroyFramebuffer(m_Device->getLogicalDevice(), framebuffer, nullptr);
    }
}

void VulkanFrameBuffer::createFrameBuffer() {
    m_FrameBuffers.resize(m_SwapChain->getSwapChainImageViews().size());

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_SwapChain->getSwapChainExtent().width;
    imageInfo.extent.height = m_SwapChain->getSwapChainExtent().height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.samples = m_Device->getMsaaSamples();
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_SwapChain->getSwapChainImageFormat();
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VulkanImage colorImage(m_Device, imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

    imageInfo.format = m_RenderPass->findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    VulkanImage depthImage(m_Device, imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

    for (size_t i = 0; i < m_SwapChain->getSwapChainImageViews().size(); i++) {
        std::array<VkImageView, 3> attachments = {colorImage.getImageView(), depthImage.getImageView(),
                                                  m_SwapChain->getSwapChainImageViews()[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass->getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_SwapChain->getSwapChainExtent().width;
        framebufferInfo.height = m_SwapChain->getSwapChainExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device->getLogicalDevice(), &framebufferInfo, nullptr, &m_FrameBuffers[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
} // namespace Engine