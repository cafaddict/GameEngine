#pragma once

#include "Log.hpp"
#include "Window.hpp"
#include <ApplicationEvent.hpp>
#include <GLFW/glfw3.h>
namespace Engine {
class Application {
public:
  Application();
  virtual ~Application();

  void run();

private:
  std::unique_ptr<Window> m_Window;
  bool m_Running = true;
};

Application *CreateApplication();
} // namespace Engine