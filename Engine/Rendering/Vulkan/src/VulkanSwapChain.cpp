#include "VulkanSwapChain.hpp"
#include "Log.hpp"
#include "VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"
namespace Engine {
VulkanSwapChain::VulkanSwapChain(std::shared_ptr<VulkanDevice> device, GLFWwindow *window)
    : m_Device(device), m_Window(window) {
    createSwapChain();
    createImageViews();
}
VulkanSwapChain::~VulkanSwapChain() {
    ENGINE_INFO("VulkanSwapChain is destroyed");
    if (destroyed)
        return;

    for (auto imageView : m_SwapChainImageViews) {
        vkDestroyImageView(m_Device->getLogicalDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_Device->getLogicalDevice(), m_SwapChain, nullptr);
}

void VulkanSwapChain::destroy() {
    for (auto imageView : m_SwapChainImageViews) {
        vkDestroyImageView(m_Device->getLogicalDevice(), imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_Device->getLogicalDevice(), m_SwapChain, nullptr);
    destroyed = true;
}

void VulkanSwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = m_Device->querySwapChainSupport(m_Device->getPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);

    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Device->getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // queueFamilyIndices indices = m_Device->findQueueFamilies(m_Device->getPhysicalDevice());
    // uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    // if (indices.graphicsFamily != indices.presentFamily) {
    //     createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    //     createInfo.queueFamilyIndexCount = 2;
    //     createInfo.pQueueFamilyIndices = queueFamilyIndices;
    //     ENGINE_WARN("Concurrent Sharing Mode");
    // } else {
    //     createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //     createInfo.queueFamilyIndexCount = 0;     // Optional
    //     createInfo.pQueueFamilyIndices = nullptr; // Optional
    //     ENGINE_WARN("Exclusive Sharing Mode");
    // }
    queueFamilyIndices indices = m_Device->findQueueFamilies(m_Device->getPhysicalDevice());

    // Ensure that both graphics and present families are valid
    if (!indices.graphicsFamily.has_value() || !indices.presentFamily.has_value()) {
        throw std::runtime_error("Failed to find required queue families!");
    }

    // Use an appropriately named array for queue family indices
    uint32_t familyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    // Check if the graphics and present families are different, use concurrent sharing mode if needed
    // if (indices.graphicsFamily != indices.presentFamily) {
    //     createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    //     createInfo.queueFamilyIndexCount = 2;
    //     createInfo.pQueueFamilyIndices = familyIndices;
    //     ENGINE_WARN("Using Concurrent Sharing Mode");
    // } else {
    //     createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //     createInfo.queueFamilyIndexCount = 0;     // Optional
    //     createInfo.pQueueFamilyIndices = nullptr; // Optional
    //     ENGINE_WARN("Using Exclusive Sharing Mode");
    // }

    std::vector<uint32_t> uniqueQueueFamilyIndices = indices.getUniqueFamilies();

    if (uniqueQueueFamilyIndices.size() > 1) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = static_cast<uint32_t>(uniqueQueueFamilyIndices.size());
        createInfo.pQueueFamilyIndices = uniqueQueueFamilyIndices.data();
        ENGINE_WARN("Using Concurrent Sharing Mode with Multiple Queue Families");
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
        ENGINE_WARN("Using Exclusive Sharing Mode");
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    if (vkCreateSwapchainKHR(m_Device->getLogicalDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_Device->getLogicalDevice(), m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device->getLogicalDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

    m_SwapChainImageFormat = surfaceFormat.format;
    m_SwapChainExtent = extent;
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(m_Window, &width, &height);
        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}

void VulkanSwapChain::createImageViews() {
    m_SwapChainImageViews.resize(m_SwapChainImages.size());
    for (uint32_t i = 0; i < m_SwapChainImages.size(); i++) {
        m_SwapChainImageViews[i] =
            createImageView(m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

VkImageView VulkanSwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                             uint32_t mipLevels) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    VkImageView imageView;
    if (vkCreateImageView(m_Device->getLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view!");
    }
    return imageView;
}
} // namespace Engine