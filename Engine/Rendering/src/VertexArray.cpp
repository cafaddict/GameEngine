#include <VertexArray.hpp>
namespace Engine {
// Vertex struct function

VertexArray::VertexArray(std::vector<Vertex> vertices) {
  m_Vertices = vertices;
  m_Attribute_num = 2;
}
unsigned int VertexArray::getAttributeNum() { return m_Attribute_num; }
inline size_t VertexArray::getVerticesSize() { return m_Vertices.size(); }
}  // namespace Engine