#pragma once
#include "TextureData.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanDescriptorSet {
    public:
    VulkanDescriptorSet() = default;
    VulkanDescriptorSet(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline,
                        std::shared_ptr<TextureData> textureData, std::vector<VkDescriptorBufferInfo> bufferInfo);
    ~VulkanDescriptorSet();

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::shared_ptr<TextureData> m_TextureData;
    VkImageView createTextureImageView(std::shared_ptr<TextureData> textureData);
    VkSampler createTextureSampler(std::shared_ptr<TextureData> textureData);

    void createDescriptorSet(VkImageView textureImageView, VkSampler textureSampler,
                             std::vector<VkDescriptorBufferInfo> bufferInfo);
};
} // namespace Engine