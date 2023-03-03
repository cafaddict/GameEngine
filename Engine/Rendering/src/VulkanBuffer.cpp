#include <VulkanBuffer.hpp>
#include <stdexcept>

namespace Engine {

VulkanBuffer::~VulkanBuffer() {}
VulkanBuffer::VulkanBuffer() {}
VulkanBuffer::VulkanBuffer(VulkanData* vulkanData) {
  m_Device = &vulkanData->device;
  m_PhysicalDevice = &vulkanData->physicalDevice;
  m_CommandPool = &vulkanData->commandPool;
  m_GraphicsQueue = &vulkanData->graphicsQueue;
}
void VulkanBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                VkMemoryPropertyFlags properties,
                                VkBuffer& buffer,
                                VkDeviceMemory& bufferMemory) {
  VkBufferCreateInfo bufferInfo{};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(*m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(*m_Device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(*m_Device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(*m_Device, buffer, bufferMemory, 0);
}

void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                              VkDeviceSize size) {
  VkCommandBuffer commandBuffer = beginSingleTimeCommands();

  VkBufferCopy copyRegion{};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanBuffer::beginSingleTimeCommands() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = *m_CommandPool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(*m_Device, &allocInfo, &commandBuffer);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
}

void VulkanBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(*m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
  vkQueueWaitIdle(*m_GraphicsQueue);

  vkFreeCommandBuffers(*m_Device, *m_CommandPool, 1, &commandBuffer);
}

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter,
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

VulkanVertexBuffer::~VulkanVertexBuffer() {}
VulkanVertexBuffer::VulkanVertexBuffer() {}
VulkanVertexBuffer::VulkanVertexBuffer(VulkanData* vulkanData,
                                       std::vector<Vertex> vertices) {
  m_Device = &vulkanData->device;
  m_PhysicalDevice = &vulkanData->physicalDevice;
  m_vertices = vertices;
  m_CommandPool = &vulkanData->commandPool;
  m_GraphicsQueue = &vulkanData->graphicsQueue;

  VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  void* data;
  vkMapMemory(*m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(*m_Device, stagingBufferMemory);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer,
      m_VertexBufferMemory);

  copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

  vkDestroyBuffer(*m_Device, stagingBuffer, nullptr);
  vkFreeMemory(*m_Device, stagingBufferMemory, nullptr);
}
void VulkanVertexBuffer::Bind() const {
  void* data;
  vkMapMemory(*m_Device, m_VertexBufferMemory, 0,
              sizeof(Vertex) * m_vertices.size(), 0, &data);
  memcpy(data, m_vertices.data(), sizeof(Vertex) * m_vertices.size());
  vkUnmapMemory(*m_Device, m_VertexBufferMemory);
}
void VulkanVertexBuffer::UnBind() const {}
void VulkanVertexBuffer::SetData(const void* data, uint32_t size){};
VulkanVertexBuffer* VulkanVertexBuffer::Create(VulkanData* vulkanData,
                                               std::vector<Vertex> vertices) {
  return new VulkanVertexBuffer(vulkanData, vertices);
}
void VulkanVertexBuffer::Destroy() {
  vkDestroyBuffer(*m_Device, m_VertexBuffer, nullptr);
  vkFreeMemory(*m_Device, m_VertexBufferMemory, nullptr);
}

VulkanIndexBuffer::~VulkanIndexBuffer() {}
VulkanIndexBuffer::VulkanIndexBuffer() {}

VulkanIndexBuffer::VulkanIndexBuffer(VulkanData* vulkanData,
                                     std::vector<uint16_t> indices) {
  m_Device = &vulkanData->device;
  m_PhysicalDevice = &vulkanData->physicalDevice;
  m_indices = indices;
  m_CommandPool = &vulkanData->commandPool;
  m_GraphicsQueue = &vulkanData->graphicsQueue;

  VkDeviceSize bufferSize = sizeof(uint16_t) * indices.size();

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingBufferMemory);

  void* data;
  vkMapMemory(*m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, indices.data(), (size_t)bufferSize);
  vkUnmapMemory(*m_Device, stagingBufferMemory);

  createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

  copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

  vkDestroyBuffer(*m_Device, stagingBuffer, nullptr);
  vkFreeMemory(*m_Device, stagingBufferMemory, nullptr);
}

void VulkanIndexBuffer::Bind() const {
  void* data;
  vkMapMemory(*m_Device, m_IndexBufferMemory, 0,
              sizeof(uint16_t) * m_indices.size(), 0, &data);
  memcpy(data, m_indices.data(), sizeof(uint16_t) * m_indices.size());
  vkUnmapMemory(*m_Device, m_IndexBufferMemory);
}
void VulkanIndexBuffer::UnBind() const {}
void VulkanIndexBuffer::SetData(const void* data, uint32_t size){};
VulkanIndexBuffer* VulkanIndexBuffer::Create(VulkanData* vulkanData,
                                             std::vector<uint16_t> indices) {
  return new VulkanIndexBuffer(vulkanData, indices);
}
void VulkanIndexBuffer::Destroy() {
  vkDestroyBuffer(*m_Device, m_IndexBuffer, nullptr);
  vkFreeMemory(*m_Device, m_IndexBufferMemory, nullptr);
}

VulkanUniformBuffer::~VulkanUniformBuffer() {}
VulkanUniformBuffer::VulkanUniformBuffer() {}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanData* vulkanData,
                                         int MAX_FRAMES_IN_FLIGHT) {
  m_Device = &vulkanData->device;
  m_PhysicalDevice = &vulkanData->physicalDevice;
  m_CommandPool = &vulkanData->commandPool;
  m_GraphicsQueue = &vulkanData->graphicsQueue;
  m_max_frame_in_flight = MAX_FRAMES_IN_FLIGHT;

  VkDeviceSize bufferSize = sizeof(UniformBufferObject);

  m_UniformBuffers.resize(m_max_frame_in_flight);
  m_UniformBuffersMemory.resize(m_max_frame_in_flight);
  m_UniformBuffersMapped.resize(m_max_frame_in_flight);

  for (size_t i = 0; i < m_max_frame_in_flight; i++) {
    createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 m_UniformBuffers[i], m_UniformBuffersMemory[i]);
    vkMapMemory(*m_Device, m_UniformBuffersMemory[i], 0, bufferSize, 0,
                &m_UniformBuffersMapped[i]);
  }
}
void VulkanUniformBuffer::Update(uint32_t currentImage,
                                 UniformBufferObject ubo) {
  m_Uniformbufferobject = ubo;
  memcpy(m_UniformBuffersMapped[currentImage], &m_Uniformbufferobject,
         sizeof(m_Uniformbufferobject));
}
void VulkanUniformBuffer::Bind() {}
void VulkanUniformBuffer::UnBind() {}
void VulkanUniformBuffer::SetData(const void* data, uint32_t size){};
VulkanUniformBuffer* VulkanUniformBuffer::Create(VulkanData* vulkanData,
                                                 int MAX_FRAMES_IN_FLIGHT) {
  return new VulkanUniformBuffer(vulkanData, MAX_FRAMES_IN_FLIGHT);
}
void VulkanUniformBuffer::Destroy() {
  for (size_t i = 0; i < m_max_frame_in_flight; i++) {
    vkDestroyBuffer(*m_Device, m_UniformBuffers[i], nullptr);
    vkFreeMemory(*m_Device, m_UniformBuffersMemory[i], nullptr);
  }
}

}  // namespace Engine