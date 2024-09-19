#pragma once
#include "VulkanGraphicsPipeline.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanDescriptorSet {
    public:
    VulkanDescriptorSet() = default;
    VulkanDescriptorSet(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline,
                        VkImageView textureImageView, VkSampler textureSampler);
    ~VulkanDescriptorSet();

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
    std::vector<VkDescriptorSet> m_DescriptorSets;
    void createDescriptorSet(VkImageView textureImageView, VkSampler textureSampler);
};
} // namespace Engine