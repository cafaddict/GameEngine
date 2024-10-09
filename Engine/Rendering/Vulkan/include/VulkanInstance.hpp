#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
namespace Engine {
class VulkanInstance {

    public:
    VkInstance getInstance() { return m_Instance; }
    VulkanInstance();
    ~VulkanInstance();

    private:
    VkInstance m_Instance;

    const bool enableValidationLayers = true;
#
    const std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    bool checkValidationLayerSupport();
    std::vector<const char *> getRequiredExtensions();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
};
} // namespace Engine