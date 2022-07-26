#include <VulkanRendering.hpp>
#include <cstring>
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include <Log.hpp>

namespace Engine {

  // Helper functions
  std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions,
      glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
      }

    return extensions;
    }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData) {
    // std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    ENGINE_ERROR("validation layer: {0}", pCallbackData->pMessage);
    return VK_FALSE;
    }

  Renderer* Renderer::Create() { return new VulkanRenderer(); }

  VulkanRenderer::VulkanRenderer() { Init(); }
  VulkanRenderer::~VulkanRenderer() { Shutdown(); }
  void VulkanRenderer::Init() { createInstance(); }
  void VulkanRenderer::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error("validation layers requested, but not available!");
      }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Renderer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "CafAddict Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto glfwExtensions = getRequiredExtensions();
    createInfo.enabledExtensionCount =
      static_cast<uint32_t>(glfwExtensions.size());
    createInfo.ppEnabledExtensionNames = glfwExtensions.data();


    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
        static_cast<uint32_t>(m_VulkanData.validationLayers.size());
      createInfo.ppEnabledLayerNames = m_VulkanData.validationLayers.data();
      }
    else {
      createInfo.enabledLayerCount = 0;
      createInfo.pNext = nullptr;
      }

    if (vkCreateInstance(&createInfo, nullptr, &m_VulkanData.instance)) {
      throw std::runtime_error("failed to create instance!");
      }

    // uint32_t extensionCount = 0;
    // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    // std::vector<VkExtensionProperties> extensions(extensionCount);
    // vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
    //   extensions.data());

    // std::cout << "available extensions:\n";

    // for (const auto& extension : extensions) {
    //   std::cout << '\t' << extension.extensionName << '\n';
    //   }
    }

  bool VulkanRenderer::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_VulkanData.validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
          }
        }

      if (!layerFound) {
        return false;
        }
      }

    return true;
    }

  void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    }


  void VulkanRenderer::setupDebugMessenger() {
    if (!enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    auto CreateDebugUtilsMessengerEXT =
      [](VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
          return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
          }
        else {
          return VK_ERROR_EXTENSION_NOT_PRESENT;
          }
      };
    if (CreateDebugUtilsMessengerEXT(m_VulkanData.instance, &createInfo, nullptr,
      &m_VulkanData.debugMessenger) !=
      VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
      }
    }

  void VulkanRenderer::Draw() {}

  void VulkanRenderer::Shutdown() {
    if (enableValidationLayers) {
      auto DestroyDebugUtilsMessengerEXT =
        [](VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
          const VkAllocationCallbacks* pAllocator) {
          auto func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
              instance, "vkDestroyDebugUtilsMessengerEXT");
          if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
            }
        };
      DestroyDebugUtilsMessengerEXT(m_VulkanData.instance,
        m_VulkanData.debugMessenger, nullptr);
      }
    vkDestroyInstance(m_VulkanData.instance, nullptr);
    }
  }  // namespace Engine
