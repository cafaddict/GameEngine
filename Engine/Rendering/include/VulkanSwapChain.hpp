#pragma once
#include "VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"
namespace Engine {
class VulkanSwapChain {
    public:
    VulkanSwapChain(std::shared_ptr<VulkanDevice> device, GLFWwindow *window);
    ~VulkanSwapChain();

    std::vector<VkImageView> getSwapChainImageViews() { return m_SwapChainImageViews; }
    VkFormat getSwapChainImageFormat() { return m_SwapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return m_SwapChainExtent; }

    private:
    std::shared_ptr<VulkanDevice> m_Device;
    GLFWwindow *m_Window;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;

    void createSwapChain();
    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
};
} // namespace Engine