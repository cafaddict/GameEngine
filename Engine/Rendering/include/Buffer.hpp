#pragma once
#include <cstdint>
namespace Engine {
class VertexBuffer {
public:
  virtual ~VertexBuffer() {}
  virtual void Bind() const = 0;
  virtual void UnBind() const = 0;
  virtual void SetData(const void *data, uint32_t size) = 0;

  // static VertexBuffer Create(float* vertices, uint32_t size);
};

class IndexBuffer {
public:
  virtual ~IndexBuffer() {}
  virtual void Bind() const = 0;
  virtual void UnBind() const = 0;
  virtual void SetData(const void *data, uint32_t size) = 0;
};

} // namespace Engine