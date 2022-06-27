#pragma once

#include <GLFW/glfw3.h>

#include <ApplicationEvent.hpp>

#include "Log.hpp"
#include "Window.hpp"
namespace Engine {
class Application {
 public:
  Application();
  virtual ~Application();

  void run();

  void OnEvent(Event& e);

 private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
};

Application* CreateApplication();
}  // namespace Engine