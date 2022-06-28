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
  for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
    (*--it)->OnEvent(e);
    if (e.IsHandled()) break;
  }
}

void Application::run() {
  while (m_Running) {
    for (Layer* layer : m_LayerStack) layer->OnUpdate();
    m_Window->OnUpdate();
  }
}

void Application::PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }

void Application::PushOverlay(Layer* overlay) {
  m_LayerStack.PushLayer(overlay);
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