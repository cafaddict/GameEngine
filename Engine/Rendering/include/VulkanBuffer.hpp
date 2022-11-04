#pragma once
#include <Buffer.hpp>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <VertexArray.hpp>
#include <cstring>
#include <VulkanData.hpp>

namespace Engine {
class VulkanVertexBuffer : public VertexBuffer {
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

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

 private:
  VkBuffer m_VertexBuffer;
  VkDeviceMemory m_VertexBufferMemory;
  VkDevice* m_Device;
  VkCommandPool* m_CommandPool;
  VkPhysicalDevice* m_PhysicalDevice;
  VkQueue* m_GraphicsQueue;

  std::vector<Vertex> m_vertices;

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
};
class VulkanIndexBuffer : public IndexBuffer {};
}  // namespace Engine