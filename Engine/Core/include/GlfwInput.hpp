#pragma once
#include "Input.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include <utility>
namespace Engine {
class GlfwInput : public Input {
 protected:
  virtual bool IsKeyPressedImpl(int keycode) override;
  virtual bool IsMouseButtonPressedImpl(int button) override;
  virtual std::pair<float, float> GetMousePositionImpl() override;
  virtual float GetMouseXImpl() override;
  virtual float GetMouseYImpl() override;
};
}  // namespace Engine