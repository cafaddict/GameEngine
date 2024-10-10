#pragma once
#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vector>
namespace Engine {
class VulkanBaseVertex {
    public:
    virtual ~VulkanBaseVertex() = default;

    // Returns the binding description for the vertex type
    virtual VkVertexInputBindingDescription getBindingDescription() const = 0;

    // Returns the attribute descriptions for the vertex type
    virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const = 0;
};
} // namespace Engine