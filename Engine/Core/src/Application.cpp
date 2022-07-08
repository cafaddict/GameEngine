#include "Application.hpp"
#include "Input.hpp"

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
Application* Application::s_Instance = nullptr;
Application::Application() {
  ENGINE_ASSERT(!s_Instance, "Application already exists!");
  s_Instance = this;
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (Layer* layer : m_LayerStack) layer->OnUpdate();

    auto [x, y] = Input::GetMousePosition();
    ENGINE_TRACE("{0}, {1}", x, y);

    m_Window->OnUpdate();
  }
}

void Application::PushLayer(Layer* layer) {
  m_LayerStack.PushLayer(layer);
  layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay) {
  m_LayerStack.PushLayer(overlay);
  overlay->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent& e) {
  bool Handled = e.IsHandled();
  if (Handled) {
    ENGINE_ASSERT(!Handled, "It is handled somewhere bug");
  }
  m_Running = false;

  return true;
}
bool Application::OnESC(KeyPressedEvent& e) {
  if ((e.GetKeyCode() == 256) && (e.GetRepeatCount() == 1)) {
    m_Running = false;
  }
  return true;
}

}  // namespace Engine