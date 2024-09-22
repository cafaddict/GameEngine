#pragma once
#include "TextureData.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanImage.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanDescriptorSet {
  public:
    VulkanDescriptorSet() = default;
    VulkanDescriptorSet(
        std::shared_ptr<VulkanDevice> device,
        std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline,
        std::shared_ptr<VulkanCommandBuffer> commandBuffer,
        std::shared_ptr<const TextureData> textureData,
        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos);
    ~VulkanDescriptorSet();

    std::vector<VkDescriptorSet> getDescriptorSets() {
        return m_DescriptorSets;
    }

  private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
    std::shared_ptr<VulkanCommandBuffer> m_CommandBuffer;
    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::shared_ptr<const TextureData> m_TextureData;
    std::shared_ptr<VulkanImage> m_Image;
    VkImageView
    createTextureImageView(std::shared_ptr<const TextureData> textureData);
    VkSampler
    createTextureSampler(std::shared_ptr<const TextureData> textureData);

    void createDescriptorSet(
        VkImageView &textureImageView, VkSampler &textureSampler,
        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos);
};
} // namespace Engine