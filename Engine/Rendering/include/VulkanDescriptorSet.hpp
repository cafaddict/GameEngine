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
    VulkanDescriptorSet(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline,
                        std::shared_ptr<VulkanCommandBuffer> commandBuffer,
                        std::shared_ptr<const TextureData> textureData,
                        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos);
    ~VulkanDescriptorSet();

    std::vector<VkDescriptorSet> getDescriptorSets() { return m_DescriptorSets; }
    VkDescriptorPool getDescriptorPool() { return m_DescriptorPool; }

    std::shared_ptr<const TextureData> m_TextureData;
    std::vector<std::vector<VkDescriptorBufferInfo>> m_BufferInfos;
    int test;

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanGraphicsPipeline> m_GraphicsPipeline;
    std::shared_ptr<VulkanCommandBuffer> m_CommandBuffer;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_DescriptorSets;

    std::shared_ptr<VulkanImage> m_Image;
    VkImageView createTextureImageView(std::shared_ptr<const TextureData> textureData);
    VkSampler createTextureSampler(std::shared_ptr<const TextureData> textureData);
    void createDescriptorPool();
    void createDescriptorSet(VkImageView &textureImageView, VkSampler &textureSampler,
                             std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfos);
};

struct DescriptorSetDataHash {
    std::size_t operator()(std::shared_ptr<const TextureData> textureData) const {
        // Use the address of TextureData for hashing
        return hashTextureData(textureData);
    }

    private:
    std::size_t hashTextureData(std::shared_ptr<const TextureData> textureData) const {
        return std::hash<const TextureData *>()(textureData.get()); // Hash the pointer (address) to TextureData
    }
};

} // namespace Engine