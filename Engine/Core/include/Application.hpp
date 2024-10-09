#pragma once

// #include <glad/glad.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <ApplicationEvent.hpp>
#include <KeyEvent.hpp>

#include "LayerStack.hpp"
#include "Log.hpp"
#include "Window.hpp"
#include "Rendering.hpp"
namespace Engine {
class Application {
    public:
    Application();
    virtual ~Application();

    void run();

    void OnEvent(Event &e);

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *overlay);

    inline static Application &Get() { return *s_Instance; }
    inline Window &GetWindow() const { return *m_Window; }
    inline Renderer *GetRenderer() const { return m_Renderer; }

    private:
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;

    LayerStack m_LayerStack;

    bool OnWindowClose(WindowCloseEvent &e);
    bool OnESC(KeyPressedEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);
    bool OnWindowMinimize(WindowIconifyEvent &e);

    static Application *s_Instance;

    // Renderer
    Renderer *m_Renderer;
    // std::unique_ptr<Shader> m_Shader;
    // std::unique_ptr<VertexBuffer> m_VertexBuffer;
};

Application *CreateApplication();
} // namespace Engine