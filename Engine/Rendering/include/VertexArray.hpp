#pragma once
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <vector>
// #include <span>
#include <array>
namespace Engine {

struct Vertex {
  glm::vec3 pos;
  glm::vec4 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
  }
  // This shoud be changed support arbitrary number of attributes
  static std::array<VkVertexInputAttributeDescription, 3>
  getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
    return attributeDescriptions;
  }
};

class VertexArray {
 public:
  VertexArray(std::vector<Vertex> vertices);
  unsigned int getAttributeNum();
  inline size_t getVerticesSize();

 private:
  std::vector<Vertex> m_Vertices;
  unsigned int m_Attribute_num;
};
}  // namespace Engine
