#include "Application.hpp"

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
Application::Application() {
  m_Window = std::unique_ptr<Window>(Window::Create());
  m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application() {}

void Application::OnEvent(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
  dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnESC));
  ENGINE_INFO("{0}", e);
}

void Application::run() {
  while (m_Running) {
    m_Window->OnUpdate();
  }
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
  m_Running = false;
  return true;
}
bool Application::OnESC(KeyPressedEvent& e) {
  if (e.GetKeyCode() == 256) {
    m_Running = false;
  }
  return true;
}

}  // namespace Engine