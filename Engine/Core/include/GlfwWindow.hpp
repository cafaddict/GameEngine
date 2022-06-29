#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Window.hpp"

namespace Engine {
class GlfwWindow : public Window {
 public:
  GlfwWindow(const WindowProps &props);
  virtual ~GlfwWindow();

  void OnUpdate() override;
  void OnWait() override;

  inline unsigned int GetWidth() const override { return m_Data.Width; }
  inline unsigned int GetHeight() const override { return m_Data.Height; }

  // Window attributes
  inline void SetEventCallback(const EventCallbackFn &callback) override {
    m_Data.EventCallback = callback;
  }
  void SetVSync(bool enabled) override;
  bool IsVSync() const override;

 private:
  GLFWwindow *m_Window;

  struct WindowData {
    std::string Title;
    unsigned int Width, Height;
    bool Vsync;
    EventCallbackFn EventCallback;
    bool WindowResized = false;
  };

  WindowData m_Data;

  virtual void Init(const WindowProps &props);
  virtual void Shutdown();
};
}  // namespace Engine