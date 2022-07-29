#pragma once
#include <GLFW/glfw3.h>
// #include <KHR/khrplatform.h>
// #include <glad/glad.h>

namespace Engine {
class Renderer {
 public:
  virtual ~Renderer() {}
  virtual void Init() = 0;
  virtual void Draw() = 0;
  virtual void SetWindow(GLFWwindow* window) = 0;
  static Renderer* Create();
  static Renderer* Create(GLFWwindow* window);
};
}  // namespace Engine