#pragma once
#include <Buffer.hpp>
#include <Vulkan/Vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <span>
#include <array>
#include <VertexArray.hpp>

namespace Engine {
class VulkanVertexBuffer : public VertexBuffer {
 public:
  virtual ~VulkanVertexBuffer();
  VulkanVertexBuffer();
  VulkanVertexBuffer(VkDevice* device, std::vector<Vertex> vertices);
  virtual void Bind() const override;
  virtual void UnBind() const override;
  virtual void SetData(const void* data, uint32_t size) override;

  static VulkanVertexBuffer* Create(VkDevice* device,
                                    std::vector<Vertex> vertices);
  void Destroy();

 private:
  VkBuffer vertexBuffer;
  VkDevice* m_Device;
};
class VulkanIndexBuffer : public IndexBuffer {};
}  // namespace Engine