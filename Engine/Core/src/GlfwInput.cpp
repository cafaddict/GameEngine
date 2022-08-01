#include "GlfwInput.hpp"

namespace Engine {
  Input* Input::s_Instance = new GlfwInput();

  bool GlfwInput::IsKeyPressedImpl(int keycode) const {
    auto window = static_cast<GLFWwindow*>(
      Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
  bool GlfwInput::IsMouseButtonPressedImpl(int button) const {
    auto window = static_cast<GLFWwindow*>(
      Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);
    return state == GLFW_PRESS;
    }

  std::pair<float, float> GlfwInput::GetMousePositionImpl() const {
    auto window = static_cast<GLFWwindow*>(
      Application::Get().GetWindow().GetNativeWindow());
    // double xpos, ypos;
    double xpos = 0;
    double ypos = 0;
    glfwGetCursorPos(window, &xpos, &xpos);
    return { (float)xpos, (float)ypos };
    }

  float GlfwInput::GetMouseXImpl() const {
    auto [x, y] = GetMousePositionImpl();
    return x;
    }

  float GlfwInput::GetMouseYImpl() const {
    auto [x, y] = GetMousePositionImpl();
    return y;
    }

  }  // namespace Engine