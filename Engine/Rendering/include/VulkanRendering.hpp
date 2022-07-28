#pragma once
#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <Rendering.hpp>

#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
namespace Engine {
  class VulkanRenderer : public Renderer {
    public:
    VulkanRenderer();
    virtual ~VulkanRenderer();

    virtual void Draw() override;

    private:
    virtual void Init() override;
    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();

    bool checkValidationLayerSupport();
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    void Shutdown();

    struct VulkanData {
      VkInstance instance;
      const std::vector<const char*> validationLayers = {
          "VK_LAYER_KHRONOS_validation" };
      VkDebugUtilsMessengerEXT debugMessenger;
      };
    VulkanData m_VulkanData;
    struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;
      bool isComplete() {
        return graphicsFamily.has_value();
        }
      };
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    };
  }  // namespace Engine