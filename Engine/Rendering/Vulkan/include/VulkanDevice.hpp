#pragma once
#include "VulkanInstance.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <vulkan/vulkan.h>
#include <set>

// struct queueFamilyIndices {
//     std::optional<uint32_t> graphicsFamily;
//     std::optional<uint32_t> computeFamily;
//     std::optional<uint32_t> transferFamily;
//     std::optional<uint32_t> presentFamily;

//     bool isComplete() {
//         return graphicsFamily.has_value() && computeFamily.has_value() &&
//                transferFamily.has_value() && presentFamily.has_value();
//     }
// };

struct queueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;
    std::optional<uint32_t> presentFamily;

    // Collect all unique families for concurrent sharing mode
    std::vector<uint32_t> getUniqueFamilies() const {
        std::set<uint32_t> uniqueFamilies;

        if (graphicsFamily.has_value())
            uniqueFamilies.insert(graphicsFamily.value());
        if (computeFamily.has_value())
            uniqueFamilies.insert(computeFamily.value());
        if (transferFamily.has_value())
            uniqueFamilies.insert(transferFamily.value());
        if (presentFamily.has_value())
            uniqueFamilies.insert(presentFamily.value());

        return std::vector<uint32_t>(uniqueFamilies.begin(), uniqueFamilies.end());
    }

    bool isComplete() const {
        return graphicsFamily.has_value() && computeFamily.has_value() && transferFamily.has_value() &&
               presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

namespace Engine {
class VulkanDevice {
    public:
    VulkanDevice(std::shared_ptr<VulkanInstance> instance, GLFWwindow *window);
    ~VulkanDevice();

    VkDevice getLogicalDevice() const { return m_LogicalDevice; }
    VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
    VkSurfaceKHR getSurface() const { return m_Surface; }
    VkSampleCountFlagBits getMsaaSamples() const { return m_MsaaSamples; }
    queueFamilyIndices getQueueFamilyIndices() const { return m_QueueFamilyIndices; }

    VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue getComputeQueue() const { return m_ComputeQueue; }
    VkQueue getTransferQueue() const { return m_TransferQueue; }
    VkQueue getPresentQueue() const { return m_PresentQueue; }

    private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSampleCountFlagBits m_MsaaSamples;
    queueFamilyIndices m_QueueFamilyIndices;

    VkQueue m_GraphicsQueue;
    VkQueue m_ComputeQueue;
    VkQueue m_TransferQueue;
    VkQueue m_PresentQueue;

    std::shared_ptr<VulkanInstance> m_Instance;
    GLFWwindow *m_Window;

    const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    private:
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSurface();

    public:
    bool isDeviceSuitable(const VkPhysicalDevice &device);
    bool checkDeviceExtensionSupport(const VkPhysicalDevice &device);
    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &device);
    int rateDeviceSuitability(const VkPhysicalDevice &device);
    queueFamilyIndices findQueueFamilies(const VkPhysicalDevice &device);
    VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDevice &device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

} // namespace Engine
