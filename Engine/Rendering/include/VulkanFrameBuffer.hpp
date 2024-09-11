#include <vulkan/vulkan.h>
#include <memory>
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"
namespace Engine {
class VulkanFrameBuffer {
    public:
    VulkanFrameBuffer(std::shared_ptr<VulkanDevice> device, std::shared_ptr<VulkanSwapChain> swapChain,
                      std::shared_ptr<VulkanRenderPass> renderPass);
    ~VulkanFrameBuffer();

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    std::shared_ptr<VulkanSwapChain> m_SwapChain;
    std::shared_ptr<VulkanRenderPass> m_RenderPass;

    std::vector<VkFramebuffer> m_FrameBuffers;
    VkExtent2D m_Extent;

    void createFrameBuffer();
};
} // namespace Engine