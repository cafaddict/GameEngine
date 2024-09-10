#pragma once
#include "VulkanInstance.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vulkan/vulkan.h>
#include <optional>

struct queueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
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

    private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    VkSampleCountFlagBits m_MsaaSamples;

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
};

} // namespace Engine
