#pragma once
#include <memory>
#include <optional>
#include <vulkan/vulkan.h>
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "vulkan/vulkan_core.h"
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
                           VulkanShadersData shaders);
    ~VulkanGraphicsPipeline();

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;

    VkPipeline m_GraphicsPipeline;
    VkPipelineLayout m_PipelineLayout;
    // VkDescriptorSetLayout m_DescriptorsetLayout;

    void createGraphicsPipeline(VulkanShadersData shaders);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void createPipelineLayout();
};
} // namespace Engine