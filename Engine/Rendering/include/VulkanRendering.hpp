#pragma once

#define GLFW_INCLUDE_VULKAN

#include <Log.hpp>

#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <Rendering.hpp>

#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <cstring>
#include <set>

#include <cstdint>    // Necessary for uint32_t
#include <limits>     // Necessary for std::numeric_limits
#include <algorithm>  // Necessary for std::clamp

#include <fstream>

namespace Engine {
class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer();
  VulkanRenderer(GLFWwindow* window);
  virtual ~VulkanRenderer();

  virtual void Draw() override;
  virtual void SetWindow(GLFWwindow* window) override {
    m_VulkanData.window = window;
  }

 private:
  // Main functions
  virtual void Init() override;
  void Shutdown();

 private:
  // struct
  struct VulkanData {
    GLFWwindow* window = nullptr;

    VkInstance instance;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
  };

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

 private:
  // Vulkan Init Data
  VulkanData m_VulkanData;

 private:
  // Init functions
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createGraphicsPipeline();

 private:
  // Helper functions
  std::vector<const char*> getRequiredExtensions();
  bool checkValidationLayerSupport();
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  bool isDeviceSuitable(VkPhysicalDevice device);
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  static std::vector<char> readFile(const std::string& filename);
  VkShaderModule createShaderModule(const std::vector<char>& code);
};
}  // namespace Engine