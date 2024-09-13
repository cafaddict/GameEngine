#pragma once
#include "VulkanBaseVertex.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
namespace Engine {
struct VulkanVertex : public VulkanBaseVertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texCoord;

    VkVertexInputBindingDescription getBindingDescription() const override {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VulkanVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const override {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VulkanVertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VulkanVertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VulkanVertex, color);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(VulkanVertex, texCoord);
        return attributeDescriptions;
    }

    bool operator==(const VulkanVertex &other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
    }
};
} // namespace Engine