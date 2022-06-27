#include "Application.hpp"

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
Application::Application() {
  m_Window = std::unique_ptr<Window>(Window::Create());
  m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application() {}

void Application::OnEvent(Event& e) { ENGINE_INFO("{0}", e); }

void Application::run() {
  while (m_Running) {
    m_Window->OnUpdate();
  }
}

}  // namespace Engine