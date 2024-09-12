#include "VulkanGraphicsPipeline.hpp"
namespace Engine {
VulkanGraphicsPipeline::VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice> device,
                                               std::shared_ptr<VulkanRenderPass> renderPass, VulkanShadersData shaders)
    : m_Device(device), m_RenderPass(renderPass) {
    createPipelineLayout();
    createGraphicsPipeline(shaders);
}
VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    vkDestroyPipeline(m_Device->getLogicalDevice(), m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_Device->getLogicalDevice(), m_PipelineLayout, nullptr);
    // vkDestroyDescriptorSetLayout(m_Device->getLogicalDevice(), m_DescriptorsetLayout, nullptr);
}

void VulkanGraphicsPipeline::createGraphicsPipeline(VulkanShadersData shaders) { m_GraphicsPipeline = VK_NULL_HANDLE; }
VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_Device->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}
void VulkanGraphicsPipeline::createPipelineLayout() { m_PipelineLayout = VK_NULL_HANDLE; }
} // namespace Engine