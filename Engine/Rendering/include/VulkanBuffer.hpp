#pragma once
#include <vulkan/vulkan.h>

#include <Buffer.hpp>
#include <VertexArray.hpp>
#include <VulkanData.hpp>
#include <array>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>

namespace Engine {

class VulkanVertexBuffer : public VertexBuffer {
 public:
  virtual ~VulkanVertexBuffer();
  VulkanVertexBuffer();
  VulkanVertexBuffer(VulkanData *vulkanData, std::vector<Vertex> vertices);
  virtual void Bind() const override;
  virtual void UnBind() const override;
  virtual void SetData(const void *data, uint32_t size) override;

  static VulkanVertexBuffer *Create(VulkanData *vulkanData,
                                    std::vector<Vertex> vertices);
  void Destroy();

  VkBuffer GetVertexBuffer() { return m_VertexBuffer; };

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

 private:
  VkBuffer m_VertexBuffer;
  VkDeviceMemory m_VertexBufferMemory;
  VkDevice *m_Device;
  VkCommandPool *m_CommandPool;
  VkPhysicalDevice *m_PhysicalDevice;
  VkQueue *m_GraphicsQueue;

  std::vector<Vertex> m_vertices;

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
};
class VulkanIndexBuffer : public IndexBuffer {
 public:
  virtual ~VulkanIndexBuffer();
  VulkanIndexBuffer();
  VulkanIndexBuffer(VulkanData *vulkanData, std::vector<uint16_t> indices);
  virtual void Bind() const override;
  virtual void UnBind() const override;
  virtual void SetData(const void *data, uint32_t size) override;

  static VulkanIndexBuffer *Create(VulkanData *vulkanData,
                                   std::vector<uint16_t> indices);
  void Destroy();

  VkBuffer GetIndexBuffer() { return m_IndexBuffer; };

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

 private:
  VkBuffer m_IndexBuffer;
  VkDeviceMemory m_IndexBufferMemory;
  VkDevice *m_Device;
  VkCommandPool *m_CommandPool;
  VkPhysicalDevice *m_PhysicalDevice;
  VkQueue *m_GraphicsQueue;

  std::vector<uint16_t> m_indices;

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
};
}  // namespace Engine