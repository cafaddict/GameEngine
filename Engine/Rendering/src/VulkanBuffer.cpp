#include <VulkanBuffer.hpp>
#include <stdexcept>

namespace Engine {
VulkanVertexBuffer::~VulkanVertexBuffer() {}
VulkanVertexBuffer::VulkanVertexBuffer() {}
VulkanVertexBuffer::VulkanVertexBuffer(VkDevice* device,
                                       VkPhysicalDevice* physicaldevice,
                                       std::vector<Vertex> vertices) {
  m_Device = device;
  m_PhysicalDevice = physicaldevice;
  m_vertices = vertices;
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(Vertex) * vertices.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(*m_Device, &bufferInfo, nullptr, &m_VertexBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create vertex buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(*m_Device, m_VertexBuffer, &memRequirements);
  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  if (vkAllocateMemory(*m_Device, &allocInfo, nullptr, &m_VertexBufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate vertex buffer memory!");
  }
}
void VulkanVertexBuffer::Bind() const {
  vkBindBufferMemory(*m_Device, m_VertexBuffer, m_VertexBufferMemory, 0);

  void* data;
  vkMapMemory(*m_Device, m_VertexBufferMemory, 0,
              sizeof(Vertex) * m_vertices.size(), 0, &data);
  memcpy(data, m_vertices.data(), sizeof(Vertex) * m_vertices.size());
  vkUnmapMemory(*m_Device, m_VertexBufferMemory);
}
void VulkanVertexBuffer::UnBind() const {}
void VulkanVertexBuffer::SetData(const void* data, uint32_t size){};
VulkanVertexBuffer* VulkanVertexBuffer::Create(VkDevice* device,
                                               VkPhysicalDevice* physicaldevice,
                                               std::vector<Vertex> vertices) {
  return new VulkanVertexBuffer(device, physicaldevice, vertices);
}
void VulkanVertexBuffer::Destroy() {
  vkDestroyBuffer(*m_Device, m_VertexBuffer, nullptr);
  vkFreeMemory(*m_Device, m_VertexBufferMemory, nullptr);
}

uint32_t VulkanVertexBuffer::findMemoryType(uint32_t typeFilter,
                                            VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(*m_PhysicalDevice, &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

}  // namespace Engine