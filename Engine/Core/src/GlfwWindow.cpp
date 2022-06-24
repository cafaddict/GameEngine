#include "GlfwWindow.hpp"
#include "Log.hpp"
namespace Engine {
static bool s_GLFWInitialized = false;
Window *Window::Create(const WindowProps &props) {
  return new GlfwWindow(props);
}

GlfwWindow::GlfwWindow(const WindowProps &props) { Init(props); }

GlfwWindow::~GlfwWindow() { Shutdown(); }

void GlfwWindow::Init(const WindowProps &props) {
  m_Data.Title = props.Title;
  m_Data.Width = props.Width;
  m_Data.Height = props.Height;

  ENGINE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width,
              props.Height);
  if (!s_GLFWInitialized) {
    int success = glfwInit();
    ENGINE_ASSERT(success, "Could not initiazlie GLFW!");
    s_GLFWInitialized = true;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    // glfwSetErrorCallback(GLFWErrorCallback);
  }

  m_Window = glfwCreateWindow((int)props.Width, (int)props.Height,
                              m_Data.Title.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(m_Window);
  glfwSetWindowUserPointer(m_Window, &m_Data);
  SetVSync(true);
}

void GlfwWindow::Shutdown() { glfwDestroyWindow(m_Window); }

void GlfwWindow::OnUpdate() {

  glfwPollEvents();
  glfwSwapBuffers(m_Window);
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

} // namespace Engine