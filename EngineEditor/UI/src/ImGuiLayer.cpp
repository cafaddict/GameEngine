#include "ImGuiLayer.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Application.hpp>

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

namespace Editor {
#define BIND_EVENT_FN(x) std::bind(&ImGuiLayer::x, this, std::placeholders::_1)
ImGuiLayer::ImGuiLayer() : Engine::Layer("ImGui Layer") {}
ImGuiLayer::~ImGuiLayer() {}
void ImGuiLayer::OnAttach() {
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
  io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

  ImGui::StyleColorsDark();

  ImGui_ImplOpenGL3_Init("#version 140");
}
void ImGuiLayer::OnDetach() {}
void ImGuiLayer::OnUpdate() {
  ImGuiIO& io = ImGui::GetIO();
  Engine::Application& app = Engine::Application::Get();
  io.DisplaySize =
      ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

  float time = (float)glfwGetTime();
  io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60);
  m_Time = time;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  static bool show = true;
  ImGui::ShowDemoWindow(&show);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void ImGuiLayer::OnWait() {}
void ImGuiLayer::OnEvent(Engine::Event& event) {
  Engine::EventDispatcher dispatcher(event);
  dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
      BIND_EVENT_FN(OnMouseButtonPressedEvent));
  dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
      BIND_EVENT_FN(OnMouseButtonReleasedEvent));
  dispatcher.Dispatch<Engine::MouseMovedEvent>(
      BIND_EVENT_FN(OnMouseMovedEvent));
  dispatcher.Dispatch<Engine::MouseScrolledEvent>(
      BIND_EVENT_FN(OnMouseScrolledEvent));
  dispatcher.Dispatch<Engine::KeyPressedEvent>(
      BIND_EVENT_FN(OnKeyPressedEvent));
  dispatcher.Dispatch<Engine::KeyReleasedEvent>(
      BIND_EVENT_FN(OnKeyReleasedEvent));
  dispatcher.Dispatch<Engine::WindowResizeEvent>(
      BIND_EVENT_FN(OnWindowResizeEvent));
  dispatcher.Dispatch<Engine::KeyTypedEvent>(BIND_EVENT_FN(OnKeyTypedEvent));
}

bool ImGuiLayer::OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseButtonEvent(e.GetMouseButton(), true);

  return false;
}
bool ImGuiLayer::OnMouseButtonReleasedEvent(
    Engine::MouseButtonReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseButtonEvent(e.GetMouseButton(), false);

  return false;
}
bool ImGuiLayer::OnMouseMovedEvent(Engine::MouseMovedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(e.GetX(), e.GetY());

  return false;
}
bool ImGuiLayer::OnMouseScrolledEvent(Engine::MouseScrolledEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());

  return false;
}
bool ImGuiLayer::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  int mods = e.GetMods();
  if (int keycode_to_mod = ImGui_ImplGlfw_KeyToModifier(e.GetKeyCode()))
    mods = (mods | keycode_to_mod);

  ImGui_ImplGlfw_UpdateKeyModifiers(mods);
  ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
  io.AddKeyEvent(imgui_key, true);

  return false;
}
bool ImGuiLayer::OnKeyReleasedEvent(Engine::KeyReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
  io.AddKeyEvent(imgui_key, false);

  return false;
}
bool ImGuiLayer::OnWindowResizeEvent(Engine::WindowResizeEvent& e) {
  return false;
}

bool ImGuiLayer::OnKeyTypedEvent(Engine::KeyTypedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddInputCharacter(e.GetCharacter());
  return false;
}

}  // namespace Editor