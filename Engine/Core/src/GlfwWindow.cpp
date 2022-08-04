#include "GlfwWindow.hpp"

#include "ApplicationEvent.hpp"
#include "KeyEvent.hpp"
#include "Log.hpp"
#include "MouseEvent.hpp"
namespace Engine {

static bool s_GLFWInitialized = false;
static void GLFWErrorCallback(int error, const char* description) {
  ENGINE_ERROR("GLFW Error ({0}): {1}", error, description);
}
Window* Window::Create(const WindowProps& props) {
  return new GlfwWindow(props);
}

GlfwWindow::GlfwWindow(const WindowProps& props) { Init(props); }

GlfwWindow::~GlfwWindow() { Shutdown(); }

void GlfwWindow::Init(const WindowProps& props) {
  m_Data.Title = props.Title;
  m_Data.Width = props.Width;
  m_Data.Height = props.Height;

  ENGINE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width,
              props.Height);
  if (!s_GLFWInitialized) {
    int success = glfwInit();
    if (!success) {
      ENGINE_ASSERT(success, "Could not initiazlie GLFW!");
    }

    s_GLFWInitialized = true;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwSetErrorCallback(GLFWErrorCallback);
  }

  m_Window = glfwCreateWindow((int)props.Width, (int)props.Height,
                              m_Data.Title.c_str(), nullptr, nullptr);
  // glfwMakeContextCurrent(m_Window); //this is for opengl
  // int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  // int status = gladLoadGL(glfwGetProcAddress);

  // ENGINE_ASSERT(status, "Failed to initialize Glad!");
  glfwSetWindowUserPointer(m_Window, &m_Data);
  // SetVSync(true); //for opengl

  // // Set GLFW callbacks
  glfwSetWindowSizeCallback(
      m_Window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;
        WindowResizeEvent event(width, height);
        data.EventCallback(event);
      });
  glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    WindowCloseEvent event;
    data.EventCallback(event);
  });

  // Custom Keycode should be made later
  glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode,
                                  int action, int mods) {
    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    switch (action) {
      case GLFW_PRESS: {
        KeyPressedEvent event(key, mods, 0);
        data.EventCallback(event);
        break;
      }

      case GLFW_RELEASE: {
        KeyReleasedEvent event(key, mods);
        data.EventCallback(event);
        break;
      }

      case GLFW_REPEAT: {
        KeyPressedEvent event(key, mods, 1);
        data.EventCallback(event);
        break;
      }
    }
  });
  glfwSetMouseButtonCallback(
      m_Window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        switch (action) {
          case GLFW_PRESS: {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            break;
          }

          case GLFW_RELEASE: {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
            break;
          }
        }
      });
  glfwSetScrollCallback(
      m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
      });
  glfwSetCursorPosCallback(
      m_Window, [](GLFWwindow* window, double xPos, double yPos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseMovedEvent event((float)xPos, (float)yPos);
        data.EventCallback(event);
      });
  glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character) {
    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    KeyTypedEvent event(character);
    data.EventCallback(event);
  });
}

void GlfwWindow::Shutdown() {
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

void GlfwWindow::OnUpdate() {
  // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT);
  glfwPollEvents();
  // glfwSwapBuffers(m_Window);
}

void GlfwWindow::OnWait() {}

void GlfwWindow::SetVSync(bool enabled) {
  if (enabled)
    glfwSwapInterval(1);
  else
    glfwSwapInterval(0);
  m_Data.Vsync = enabled;
}

bool GlfwWindow::IsVSync() const { return m_Data.Vsync; }

}  // namespace Engine