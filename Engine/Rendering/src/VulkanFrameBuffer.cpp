#include "VulkanFrameBuffer.hpp"
#include "Log.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include <array>
#include <memory>
namespace Engine {

VulkanFrameBuffer::VulkanFrameBuffer(
    std::shared_ptr<VulkanDevice> device,
    std::shared_ptr<VulkanSwapChain> swapChain,
    std::shared_ptr<VulkanRenderPass> renderPass,
    std::shared_ptr<VulkanCommandBuffer> commandBuffer)
    : m_Device(device), m_SwapChain(swapChain), m_RenderPass(renderPass) {
    createFrameBuffer();
}

VulkanFrameBuffer::~VulkanFrameBuffer() {
    ENGINE_INFO("VulkanFrameBuffer is destroyed");
    for (auto framebuffer : m_FrameBuffers) {
        vkDestroyFramebuffer(m_Device->getLogicalDevice(), framebuffer,
                             nullptr);
    }
}

void VulkanFrameBuffer::createFrameBuffer() {
    m_FrameBuffers.resize(m_SwapChain->getSwapChainImageViews().size());

    VkImageCreateInfo colorImageInfo{};
    colorImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    colorImageInfo.imageType = VK_IMAGE_TYPE_2D;
    colorImageInfo.extent.width = m_SwapChain->getSwapChainExtent().width;
    colorImageInfo.extent.height = m_SwapChain->getSwapChainExtent().height;
    colorImageInfo.extent.depth = 1;
    colorImageInfo.mipLevels = 1;
    colorImageInfo.samples = m_Device->getMsaaSamples();
    colorImageInfo.arrayLayers = 1;
    colorImageInfo.format = m_SwapChain->getSwapChainImageFormat();
    colorImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorImageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                           VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    m_ColorImage = std::make_shared<VulkanImage>(
        m_Device, colorImageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT);

    VkImageCreateInfo depthImageInfo{};
    depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
    depthImageInfo.extent.width = m_SwapChain->getSwapChainExtent().width;
    depthImageInfo.extent.height = m_SwapChain->getSwapChainExtent().height;
    depthImageInfo.extent.depth = 1;
    depthImageInfo.mipLevels = 1;
    depthImageInfo.samples = m_Device->getMsaaSamples();
    depthImageInfo.arrayLayers = 1;
    depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    depthImageInfo.format = m_RenderPass->findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
         VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    m_DepthImage = std::make_shared<VulkanImage>(
        m_Device, m_CommandBuffer, depthImageInfo,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

    for (size_t i = 0; i < m_SwapChain->getSwapChainImageViews().size(); i++) {

        std::array<VkImageView, 3> attachments = {
            m_ColorImage->getImageView(), m_DepthImage->getImageView(),
            m_SwapChain->getSwapChainImageViews()[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass->getRenderPass();
        framebufferInfo.attachmentCount =
            static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_SwapChain->getSwapChainExtent().width;
        framebufferInfo.height = m_SwapChain->getSwapChainExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device->getLogicalDevice(), &framebufferInfo,
                                nullptr, &m_FrameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
} // namespace Engine