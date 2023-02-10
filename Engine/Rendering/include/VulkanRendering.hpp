#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <Log.hpp>
#include <Rendering.hpp>
#include <VertexArray.hpp>
#include <VulkanBuffer.hpp>
#include <VulkanData.hpp>
#include <VulkanShader.hpp>
#include <algorithm>  // Necessary for std::clamp
#include <cstdint>    // Necessary for uint32_t
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>  // Necessary for std::numeric_limits
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

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
  virtual void SetWindowResized(bool resized) override {
    m_VulkanData.framebufferResized = resized;
  }
  virtual void SetWindowMinimized(bool minimized) override {
    m_VulkanData.minimized = minimized;
  }

  virtual void WaitIdle() override { vkDeviceWaitIdle(m_VulkanData.device); }

 private:
  // Main functions
  virtual void Init() override;
  void recreateSwapChain();
  void Shutdown();

 private:
  // struct

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
  std::unique_ptr<VulkanVertexBuffer> m_VertexBuffer;
  std::unique_ptr<VulkanIndexBuffer> m_IndexBuffer;

  const int MAX_FRAMES_IN_FLIGHT = 2;

 private:
  // Init functions
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createRenderPass();
  void createGraphicsPipeline();
  void createFramebuffer();
  void createCommandPool();
  void createVertexBuffer();
  void createIndexBuffer();
  void createCommandBuffer();
  void createSyncObjects();

  void cleanupSwapChain();

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

  VkShaderModule createShaderModule(const std::vector<char>& code);

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

 public:
  static std::vector<char> readFile(const std::string_view filename);
};
}  // namespace Engine