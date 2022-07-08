#pragma once
#include <imgui/imgui.h>

#include <Event.hpp>
#include <KeyEvent.hpp>
#include <MouseEvent.hpp>
#include <ApplicationEvent.hpp>
#include <Layer.hpp>
#include <Log.hpp>

namespace Editor {
class ImGuiLayer : public Engine::Layer {
 public:
  ImGuiLayer();
  ~ImGuiLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate() override;
  virtual void OnWait() override;
  virtual void OnEvent(Engine::Event& event) override;

 private:
  float m_Time = 0.0f;

 private:
  bool OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent& e);
  bool OnMouseButtonReleasedEvent(Engine::MouseButtonReleasedEvent& e);
  bool OnMouseMovedEvent(Engine::MouseMovedEvent& e);
  bool OnMouseScrolledEvent(Engine::MouseScrolledEvent& e);
  bool OnKeyPressedEvent(Engine::KeyPressedEvent& e);
  bool OnKeyReleasedEvent(Engine::KeyReleasedEvent& e);
  bool OnWindowResizeEvent(Engine::WindowResizeEvent& e);
  bool OnKeyTypedEvent(Engine::KeyTypedEvent& e);
};
}  // namespace Editor