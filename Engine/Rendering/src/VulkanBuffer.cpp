#include <VulkanBuffer.hpp>
#include <stdexcept>

namespace Engine {
VulkanVertexBuffer::~VulkanVertexBuffer() {}
VulkanVertexBuffer::VulkanVertexBuffer() {}
VulkanVertexBuffer::VulkanVertexBuffer(VkDevice* device,
                                       std::vector<Vertex> vertices) {
  m_Device = device;
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(Vertex) * vertices.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(*m_Device, &bufferInfo, nullptr, &vertexBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }
}
void VulkanVertexBuffer::Bind() const {}
void VulkanVertexBuffer::UnBind() const {}
void VulkanVertexBuffer::SetData(const void* data, uint32_t size){};
VulkanVertexBuffer* VulkanVertexBuffer::Create(VkDevice* device,
                                               std::vector<Vertex> vertices) {
  return new VulkanVertexBuffer(device, vertices);
}
void VulkanVertexBuffer::Destroy() {
  vkDestroyBuffer(*m_Device, vertexBuffer, nullptr);
}

}  // namespace Engine