#include <vulkan/vulkan.h>
#include <memory>
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
#include "vulkan/vulkan_core.h"
namespace Engine {
class VulkanFrameBuffer {
    public:
    VulkanFrameBuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapChain,
                      std::shared_ptr<VulkanRenderPass> renderPass);
    ~VulkanFrameBuffer();

    std::vector<VkFramebuffer> getFrameBuffers() { return m_FrameBuffers; }

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanSwapChain> m_SwapChain;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;

    std::vector<VkFramebuffer> m_FrameBuffers;
    VkExtent2D m_Extent;

    void createFrameBuffer();
};
} // namespace Engine