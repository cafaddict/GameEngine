#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.hpp"

namespace Engine {
class GlfwWindow : public Window {
 public:
  GlfwWindow(const WindowProps &props);
  virtual ~GlfwWindow();

  virtual void OnUpdate() override;
  virtual void OnWait() override;

  inline virtual unsigned int GetWidth() const override { return m_Data.Width; }
  inline virtual unsigned int GetHeight() const override {
    return m_Data.Height;
  }

  // Window attributes
  inline virtual void SetEventCallback(
      const EventCallbackFn &callback) override {
    m_Data.EventCallback = callback;
  }
  void virtual SetVSync(bool enabled) override;
  bool virtual IsVSync() const override;

  inline virtual void *GetNativeWindow() const override { return m_Window; };

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