#pragma once
#include "Input.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.hpp"
#include <utility>
namespace Engine {
  class GlfwInput : public Input {
    protected:
    virtual bool IsKeyPressedImpl(int keycode) const override;
    virtual bool IsMouseButtonPressedImpl(int button) const override;
    virtual std::pair<float, float> GetMousePositionImpl() const override;
    virtual float GetMouseXImpl() const override;
    virtual float GetMouseYImpl() const override;
    };
  }  // namespace Engine