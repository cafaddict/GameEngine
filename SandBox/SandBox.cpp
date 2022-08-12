#include <Engine.hpp>
#include <ImGuiLayer.hpp>
#include <Input.hpp>

class ExampleLayer : public Engine::Layer {
 public:
  ExampleLayer() : Layer("Example") {}
  void OnUpdate() override {
    // CLIENT_INFO("ExampleLayer::Update");
    if (Engine::Input::IsKeyPressed(GLFW_KEY_A)) {
      CLIENT_TRACE("A key is pressed");
    }
  }
  void OnEvent(Engine::Event& event) override {
    // CLIENT_INFO("{0}", event);
  }
};

class Sandbox : public Engine::Application {
 public:
  Sandbox() {
    PushLayer(new ExampleLayer());
    // PushOverlay(new Editor::ImGuiLayer());
  }
  ~Sandbox() {}
};

Engine::Application* Engine::CreateApplication() { return new Sandbox(); }