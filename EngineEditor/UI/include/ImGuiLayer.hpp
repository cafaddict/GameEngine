#pragma once
#include <imgui/imgui.h>

#include <Event.hpp>
#include <Layer.hpp>
#include <Log.hpp>

namespace Editor {
class ImGuiLayer : public Engine::Layer {
 public:
  ImGuiLayer();
  ~ImGuiLayer();

  void OnAttach();
  void OnDetach();
  void OnUpdate();
  void OnWait();
  void OnEvent(Engine::Event &event);

 private:
  float m_Time = 0.0f;
};
}  // namespace Editor