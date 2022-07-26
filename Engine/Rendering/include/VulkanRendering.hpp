#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <Rendering.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
namespace Engine {
class VulkanRenderer : public Renderer {
 public:
  VulkanRenderer();
  virtual ~VulkanRenderer();

  virtual void Draw() override;

 private:
  virtual void Init() override;
  void createInstance();
  bool checkValidationLayerSupport();
  void setupDebugMessenger();

  void Shutdown();

  struct VulkanData {
    VkInstance instance;
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
    VkDebugUtilsMessengerEXT debugMessenger;
  };
  VulkanData m_VulkanData;
};
}  // namespace Engine