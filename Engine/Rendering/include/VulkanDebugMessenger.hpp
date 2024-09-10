#pragma once
#include <VulkanInstance.hpp>
#include <memory>
#include <vulkan/vulkan.h>

namespace Engine {
class VulkanDebugMessenger {
    public:
    VulkanDebugMessenger(std::shared_ptr<VulkanInstance> instance);
    ~VulkanDebugMessenger();

    private:
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    std::shared_ptr<VulkanInstance> m_Instance; // Use shared_ptr for instance

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator,
                                          VkDebugUtilsMessengerEXT *pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    const bool enableValidationLayers = true;
};
} // namespace Engine