#pragma once
#include <vulkan/vulkan.h>
#include "VulkanDevice.hpp"
namespace Engine {
class VulkanGraphicsPipeline {
    public:
    VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice> device, VkExtent2D extent);
    ~VulkanGraphicsPipeline();

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    VkPipeline m_GraphicsPipeline;
    VkPipelineLayout m_PipelineLayout;
    VkRenderPass m_RenderPass;
    VkExtent2D m_Extent;

    void createRenderPass();
    void createGraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createPipelineLayout();
};
} // namespace Engine