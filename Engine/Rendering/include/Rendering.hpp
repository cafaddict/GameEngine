#pragma once
// #include <KHR/khrplatform.h>
// #include <glad/glad.h>

namespace Engine {
class Renderer {
 public:
  virtual ~Renderer() {}
  virtual void Init() {}
  virtual void Draw() {}
  static Renderer *Create();
};
}  // namespace Engine