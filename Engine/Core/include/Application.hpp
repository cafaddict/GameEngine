#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ApplicationEvent.hpp>
#include <KeyEvent.hpp>

#include "LayerStack.hpp"
#include "Log.hpp"
#include "Window.hpp"
namespace Engine {
class Application {
 public:
  Application();
  virtual ~Application();

  void run();

  void OnEvent(Event& e);

  void PushLayer(Layer* layer);
  void PushOverlay(Layer* overlay);

  inline static Application& Get() { return *s_Instance; }
  inline Window& GetWindow() { return *m_Window; }

 private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;

  LayerStack m_LayerStack;

  bool OnWindowClose(WindowCloseEvent& e);
  bool OnESC(KeyPressedEvent& e);

  static Application* s_Instance;
};

Application* CreateApplication();
}  // namespace Engine