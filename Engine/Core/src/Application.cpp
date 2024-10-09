#include "Application.hpp"
#include "Input.hpp"
#include "Log.hpp"
#include "OpenGLRenderer.hpp"
#include "VulkanRenderer.hpp"

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
Application *Application::s_Instance = nullptr;
Application::Application() {
    ENGINE_WARN("Application");
    ENGINE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    // m_Window = std::unique_ptr<Window>(Window::Create());
    // m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    // Renderer
    // auto window = static_cast<GLFWwindow *>(m_Window->GetNativeWindow());
    // m_Renderer = Renderer::Create(window);
}

void Application::SetWindow(RendererType rendertype) {
    WindowProps props;
    props.Title = "Initial Application";
    props.Width = 1200;
    props.Height = 800;
    props.Renderer = rendertype;
    m_Window = std::unique_ptr<Window>(Window::Create(props));
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

void Application::SetRenderer(RendererType rendertype) {
    auto window = static_cast<GLFWwindow *>(m_Window->GetNativeWindow());
    if (rendertype == RendererType::OpenGL) {
        m_Renderer = OpenGLRenderer::Create(window);
    } else if (rendertype == RendererType::Vulkan) {
        m_Renderer = VulkanRenderer::Create(window);
    }
}

Application::~Application() {}

void Application::OnEvent(Event &e) {
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<WindowIconifyEvent>(BIND_EVENT_FN(OnWindowMinimize));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnESC));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
    // ENGINE_INFO("{0}", e);
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
        (*--it)->OnEvent(e);
        if (e.IsHandled())
            break;
    }
}

void Application::run() {
    while (m_Running) {
        m_Renderer->BeginRecord();
        for (Layer *layer : m_LayerStack)
            layer->OnUpdate();
        m_Renderer->EndRecord();

        m_Window->OnUpdate();

        // m_Renderer->Draw();
    }

    m_Renderer->WaitIdle();
}

void Application::PushLayer(Layer *layer) {
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer *overlay) {
    m_LayerStack.PushLayer(overlay);
    overlay->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent &e) {
    bool Handled = e.IsHandled();
    if (Handled) {
        ENGINE_ASSERT(!Handled, "It is handled somewhere bug");
    }
    m_Running = false;

    return true;
}
bool Application::OnESC(KeyPressedEvent &e) {
    bool Handled = e.IsHandled();
    if (Handled) {
        ENGINE_ASSERT(!Handled, "It is handled somewhere bug");
    }
    if ((e.GetKeyCode() == 256) && (e.GetRepeatCount() == 1)) {
        m_Running = false;
    }
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent &e) {
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

bool Application::OnWindowMinimize(WindowIconifyEvent &e) {
    bool Handled = e.IsHandled();
    if (Handled) {
        ENGINE_ASSERT(!Handled, "It is handled somewhere bug");
    }

    if (e.IsMinimized()) {
        // Window was minimized
        ENGINE_INFO("Window is minimized");
        m_Renderer->SetWindowMinimized(true);

    } else {
        // Window was restored
        ENGINE_INFO("Window is restored");
        m_Renderer->SetWindowMinimized(false);
    }
    return true;
}

} // namespace Engine