#pragma once
#include <vulkan/vulkan.h>

#include <Buffer.hpp>
#include <VertexArray.hpp>
#include <VulkanData.hpp>
#include <array>
#include <chrono>
#include <cstring>
#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include <Particle.hpp>

namespace Engine {

    struct UniformBufferObject {
        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
        };

    class VulkanBuffer {
        public:
        virtual ~VulkanBuffer() = default;
        VulkanBuffer();
        VulkanBuffer(VulkanData* vulkanData);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties, VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        uint32_t findMemoryType(uint32_t typeFilter,
            VkMemoryPropertyFlags properties);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);



        protected:
        VkDevice* m_Device;
        VkCommandPool* m_CommandPool;
        VkPhysicalDevice* m_PhysicalDevice;
        VkQueue* m_GraphicsQueue;
        };

    class VulkanVertexBuffer : public VertexBuffer, private VulkanBuffer {
        public:
        virtual ~VulkanVertexBuffer();
        VulkanVertexBuffer();
        VulkanVertexBuffer(VulkanData* vulkanData, std::vector<Vertex> vertices);
        virtual void Bind() const override;
        virtual void UnBind() const override;
        virtual void SetData(const void* data, uint32_t size) override;

        static VulkanVertexBuffer* Create(VulkanData* vulkanData,
            std::vector<Vertex> vertices);
        void Destroy();

        VkBuffer GetVertexBuffer() { return m_VertexBuffer; };

        private:
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;

        std::vector<Vertex> m_vertices;

        uint32_t findMemoryType(uint32_t typeFilter,
            VkMemoryPropertyFlags properties);
        };
    class VulkanIndexBuffer : public IndexBuffer, private VulkanBuffer {
        public:
        virtual ~VulkanIndexBuffer();
        VulkanIndexBuffer();
        VulkanIndexBuffer(VulkanData* vulkanData, std::vector<uint32_t> indices);
        virtual void Bind() const override;
        virtual void UnBind() const override;
        virtual void SetData(const void* data, uint32_t size) override;

        static VulkanIndexBuffer* Create(VulkanData* vulkanData,
            std::vector<uint32_t> indices);
        void Destroy();

        VkBuffer GetIndexBuffer() { return m_IndexBuffer; };

        private:
        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferMemory;

        std::vector<uint32_t> m_indices;
        };

    class VulkanUniformBuffer : private VulkanBuffer {


        public:
        struct CameraUBO {
            glm::mat4 view;
            glm::mat4 proj;
            };

        struct LightUBO {
            glm::vec3 lightPos;
            glm::vec3 lightColor;
            };

        public:
        ~VulkanUniformBuffer();
        VulkanUniformBuffer();
        VulkanUniformBuffer(VulkanData* vulkanData, int MAX_FRAMES_IN_FLIGHT);
        void Update(uint32_t currentImage, UniformBufferObject ubo);
        void UpdateCamera(uint32_t currentImage, const CameraUBO& ubo);
        void UpdateLight(uint32_t currentImage, const LightUBO& ubo);
        void Bind();
        void UnBind();
        void SetData(const void* data, uint32_t size);
        static VulkanUniformBuffer* Create(VulkanData* vulkanData,

            int MAX_FRAMES_IN_FLIGHT);
        void Destroy();
        std::vector<VkBuffer> GetUniformBuffers() { return m_UniformBuffers; }
        std::vector<VkBuffer> GetCameraBuffers() { return m_CameraBuffers; }
        std::vector<VkBuffer> GetLightBuffers() { return m_LightBuffers; }

        private:
        std::vector<VkBuffer> m_UniformBuffers;
        std::vector<VkDeviceMemory> m_UniformBuffersMemory;
        std::vector<void*> m_UniformBuffersMapped;

        std::vector<VkBuffer> m_CameraBuffers;
        std::vector<VkDeviceMemory> m_CameraBuffersMemory;
        std::vector<void*> m_CameraBuffersMapped;

        std::vector<VkBuffer> m_LightBuffers;
        std::vector<VkDeviceMemory> m_LightBuffersMemory;
        std::vector<void*> m_LightBuffersMapped;

        UniformBufferObject m_Uniformbufferobject;
        int m_max_frame_in_flight;
        };


    template<typename T>
    class VulkanShaderStorageBuffer : public VulkanBuffer {
        public:
        ~VulkanShaderStorageBuffer();
        VulkanShaderStorageBuffer();
        VulkanShaderStorageBuffer(VulkanData* vulkanData, int MAX_FRAMES_IN_FLIGHT, VkDeviceSize buffersize);
        void Bind();
        void UnBind();
        void SetData(const void* data, uint32_t size);
        static VulkanShaderStorageBuffer* Create(VulkanData* vulkanData,
            int MAX_FRAMES_IN_FLIGHT, VkDeviceSize buffersize);
        void Destroy();
        std::vector<VkBuffer> GetShaderStorageBuffers() { return m_ShaderStorageBuffers; }
        void Update(uint32_t currentImage, const std::vector<T>& objects);

        private:
        std::vector<VkBuffer> m_ShaderStorageBuffers;
        std::vector<VkDeviceMemory> m_ShaderStorageBuffersMemory;
        std::vector<void*> m_ShaderStorageBuffersMapped;

        int m_max_frame_in_flight;
        VkDeviceSize m_BufferSize;
        };
    }  // namespace Engine

#include "VulkanBuffer.tpp"