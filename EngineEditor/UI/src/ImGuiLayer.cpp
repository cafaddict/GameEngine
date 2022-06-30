#include "ImGuiLayer.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <Application.hpp>

#include "imgui_impl_opengl3.h"

namespace Editor {
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
void ImGuiLayer::OnEvent(Engine::Event& event) {}

}  // namespace Editor