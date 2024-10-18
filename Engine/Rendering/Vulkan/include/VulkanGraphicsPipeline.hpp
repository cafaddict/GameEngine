#pragma once
#include "VulkanBaseVertex.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanVertex.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <optional>
#include <vulkan/vulkan.h>
namespace Engine {

struct VulkanShadersData {
    std::optional<std::vector<char>> vertexShader;
    std::optional<std::vector<char>> fragShader;
    std::optional<std::vector<char>> computeShader;

    // Define the equality operator
    bool operator==(const VulkanShadersData &other) const {
        return vertexShader == other.vertexShader && fragShader == other.fragShader &&
               computeShader == other.computeShader;
    }
};

struct ShaderDataHash {
    std::size_t operator()(const VulkanShadersData &shaders) const {
        // Combine hash values of vertex, fragment, and compute shaders
        std::size_t h1 =
            shaders.vertexShader ? hashVector(*shaders.vertexShader) : 0; // Vertex shader hash or 0 if missing
        std::size_t h2 =
            shaders.fragShader ? hashVector(*shaders.fragShader) : 0; // Fragment shader hash or 0 if missing
        std::size_t h3 =
            shaders.computeShader ? hashVector(*shaders.computeShader) : 0; // Compute shader hash or 0 if missing
        return h1 ^ (h2 << 1) ^ (h3 << 2);                                  // Combine hash values
    }

    private:
    std::size_t hashVector(const std::vector<char> &data) const {
        std::size_t hash = 0;
        for (const char &byte : data) {
            hash ^= std::hash<char>()(byte) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};
class VulkanGraphicsPipeline {
    public:
    VulkanGraphicsPipeline() = default; // Default constructor

    VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanRenderPass> renderPass,
                           VulkanShadersData shaders, VulkanBaseVertex &vertexType);
    VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanRenderPass> renderPass,
                           VulkanShadersData shaders, VulkanBaseVertex &vertexType, size_t numTextures);
    VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanRenderPass> renderPass,
                           VulkanShadersData shaders, VulkanBaseVertex &vertexType, bool isPBR);
    ~VulkanGraphicsPipeline();
    VkDescriptorSetLayout getDescriptorSetLayout() { return m_DescriptorsetLayout; }
    VkDescriptorSetLayout getComputeDescriptorSetLayout() { return m_ComputeDescriptorsetLayout; }
    VkDescriptorSetLayout getPBRDescriptorSetLayout() { return m_PBRDescriptorsetLayout; }

    VkPipeline getGraphicsPipeline() { return m_GraphicsPipeline; }
    VkPipelineLayout getPipelineLayout() { return m_PipelineLayout; }
    VulkanShadersData getShaders() { return m_Shaders; }

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;
    VulkanShadersData m_Shaders;

    VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
    VkPipeline m_ComputePipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayout m_ComputePipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_DescriptorsetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_PBRDescriptorsetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_ComputeDescriptorsetLayout = VK_NULL_HANDLE;

    void createGraphicsPipeline(VulkanShadersData shaders, VulkanBaseVertex &vertexType);
    void createComputePipeline(VulkanShadersData shaders);
    void createPBRRPipeline(VulkanShadersData shaders, VulkanBaseVertex &vertexType);

    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout);
    void createDescriptorSetLayout();
    void createPBRDescriptorSetLayout();

    void createDescriptorSetLayout(size_t numTextures);

    void createComputeDescriptorSetLayout();
};
} // namespace Engine