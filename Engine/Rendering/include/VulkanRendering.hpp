#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <Log.hpp>
#include <Rendering.hpp>
#include <VertexArray.hpp>
#include <VulkanBuffer.hpp>
#include <VulkanData.hpp>
#include <VulkanShader.hpp>
#include <algorithm> // Necessary for std::clamp
#include <cstdint>   // Necessary for uint32_t
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits> // Necessary for std::numeric_limits
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
// #include <stb_image/stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
// #include <tinyobjloader/tiny_obj_loader.h>

#include <Particle.hpp>

namespace Engine
    {

    // const std::vector<Vertex> vertices = {
    //     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    //     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    //     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    //     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    //     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    //     {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    //     {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    //     {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

    // const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

    const std::string MODEL_PATH = "../../resources/models/viking_room.obj";
    const std::string TEXTURE_PATH = "../../resources/models/viking_room.png";

    class VulkanRenderer : public Renderer
        {
            public:
            VulkanRenderer();
            VulkanRenderer(GLFWwindow* window);
            virtual ~VulkanRenderer();

            void BeginRecord();
            virtual void addModel(std::string model_path) override;
            virtual void addParticles(std::vector<Particle> particles) override;
            virtual void Draw() override;
            virtual void SetWindow(GLFWwindow* window) override
                {
                m_VulkanData.window = window;
                }
            virtual void SetWindowResized(bool resized) override
                {
                m_VulkanData.framebufferResized = resized;
                }
            virtual void SetWindowMinimized(bool minimized) override
                {
                m_VulkanData.minimized = minimized;
                }

            virtual void WaitIdle() override { vkDeviceWaitIdle(m_VulkanData.device); }

            VulkanData GetVulkanData() { return m_VulkanData; }

            public:
            // Main functions
            virtual void Init() override;
            void recreateSwapChain();
            void Shutdown();


            // struct

            struct QueueFamilyIndices
                {
                std::optional<uint32_t> graphicsFamily;
                std::optional<uint32_t> presentFamily;

                bool isComplete()
                    {
                    return graphicsFamily.has_value() && presentFamily.has_value();
                    }
                };

            struct SwapChainSupportDetails
                {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector<VkSurfaceFormatKHR> formats;
                std::vector<VkPresentModeKHR> presentModes;
                };

            private:
            // Vulkan Init Data
            VulkanData m_VulkanData;
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::unique_ptr<VulkanVertexBuffer> m_VertexBuffer;
            std::unique_ptr<VulkanIndexBuffer> m_IndexBuffer;
            std::unique_ptr<VulkanUniformBuffer> m_UniformBuffers;

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
            void createGUIRenderPass();
            void createDescriptorSetLayout();
            void createGraphicsPipeline();
            void createCommandPool();
            void createColorResources();
            void createDepthResources();
            void createFramebuffer();
            void createGUIFramebuffer();
            void createTextureImage();
            void createTextureImageView();
            void createTextureSampler();
            void loadModel();
            void createVertexBuffer();
            void createIndexBuffer();
            void createUniformBuffers();
            void createDescriptorPool();
            void createDescriptorSets();
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


            // Image Related Helper function
            void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                VkSampleCountFlagBits numSamples, VkFormat format,
                VkImageTiling tiling, VkImageUsageFlags usage,
                VkMemoryPropertyFlags properties, VkImage& image,
                VkDeviceMemory& imageMemory);
            void transitionImageLayout(VkImage image, VkFormat format,
                VkImageLayout oldLayout, VkImageLayout newLayout,
                uint32_t mipLevels);
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                uint32_t height);
            VkImageView createImageView(VkImage image, VkFormat format,
                VkImageAspectFlags aspectFlags,
                uint32_t mipLevels);

            VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                VkImageTiling tiling,
                VkFormatFeatureFlags features);

            VkFormat findDepthFormat();

            bool hasStencilComponent(VkFormat format);

            void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                int32_t texHeight, uint32_t mipLevels);

            VkSampleCountFlagBits getMaxUsableSampleCount();

            public:
            static std::vector<char> readFile(const std::string_view filename);

            private:
            // will be removed
            void updateUniformBuffer(uint32_t currentImage)
                {
                static auto startTime = std::chrono::high_resolution_clock::now();

                auto currentTime = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration<float, std::chrono::seconds::period>(
                    currentTime - startTime)
                    .count();

                UniformBufferObject ubo{};
                ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
                ubo.view =
                    glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f));
                ubo.proj = glm::perspective(glm::radians(45.0f),
                    m_VulkanData.swapChainExtent.width /
                    (float)m_VulkanData.swapChainExtent.height,
                    0.1f, 10.0f);
                ubo.proj[1][1] *= -1;
                m_UniformBuffers->Update(currentImage, ubo);
                }
        };
    } // namespace Engine