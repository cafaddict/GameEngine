#pragma once
#include <Buffer.hpp>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <VertexArray.hpp>
#include <cstring>

namespace Engine {
class VulkanVertexBuffer : public VertexBuffer {
 public:
  virtual ~VulkanVertexBuffer();
  VulkanVertexBuffer();
  VulkanVertexBuffer(VkDevice* device, VkPhysicalDevice* physicaldevice,
                     std::vector<Vertex> vertices);
  virtual void Bind() const override;
  virtual void UnBind() const override;
  virtual void SetData(const void* data, uint32_t size) override;

  static VulkanVertexBuffer* Create(VkDevice* device,
                                    VkPhysicalDevice* physicaldevice,
                                    std::vector<Vertex> vertices);
  void Destroy();

  VkBuffer GetVertexBuffer() { return m_VertexBuffer; };

 private:
  VkBuffer m_VertexBuffer;
  VkDeviceMemory m_VertexBufferMemory;
  VkDevice* m_Device;
  VkPhysicalDevice* m_PhysicalDevice;
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  std::vector<Vertex> m_vertices;
};
class VulkanIndexBuffer : public IndexBuffer {};
}  // namespace Engine