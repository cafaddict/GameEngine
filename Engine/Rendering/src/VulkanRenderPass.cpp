#include "VulkanRenderPass.hpp"
namespace Engine {
VulkanRenderPass::VulkanRenderPass(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapChain)
    : m_Device(device), m_SwapChain(swapChain) {
    createRenderPass();
}

VulkanRenderPass::~VulkanRenderPass() { vkDestroyRenderPass(m_Device->getLogicalDevice(), m_RenderPass, nullptr); }

void VulkanRenderPass::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_SwapChain->getSwapChainImageFormat();
    colorAttachment.samples = m_Device->getMsaaSamples();
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}
} // namespace Engine