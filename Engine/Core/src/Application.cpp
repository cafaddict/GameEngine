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

  // Renderer
  auto window = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
  // m_Renderer->SetWindow(window);
  m_Renderer = Renderer::Create(window);
}

Application::~Application() {}

void Application::OnEvent(Event& e) {
  EventDispatcher dispatcher(e);
  dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
  dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnESC));
  dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
  // ENGINE_INFO("{0}", e);
  for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
    (*--it)->OnEvent(e);
    if (e.IsHandled()) break;
  }
}

void Application::run() {
  while (m_Running) {
    for (Layer* layer : m_LayerStack) layer->OnUpdate();

    m_Window->OnUpdate();

    m_Renderer->Draw();
    ;
  }

  m_Renderer->WaitIdle();
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
  bool Handled = e.IsHandled();
  if (Handled) {
    ENGINE_ASSERT(!Handled, "It is handled somewhere bug");
  }
  if ((e.GetKeyCode() == 256) && (e.GetRepeatCount() == 1)) {
    m_Running = false;
  }
  return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e) {
  bool Handled = e.IsHandled();
  if (Handled) {
    ENGINE_ASSERT(!Handled, "It is handled somewhere bug");
  }
  ENGINE_INFO("{0}", e);
  m_Renderer->SetWindowResized(true);

  if (e.GetWidth() == 0 || e.GetHeight() == 0) {
    m_Renderer->SetWindowMinimized(true);
  } else {
    m_Renderer->SetWindowMinimized(false);
  }
  return true;
}

}  // namespace Engine