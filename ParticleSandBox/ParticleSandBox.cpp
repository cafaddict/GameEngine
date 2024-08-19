#define GLFW_INCLUDE_NONE
#include <Engine.hpp>
// #include <ImGuiLayer.hpp>
// #include <Input.hpp>
// #include <Application.hpp>
// #include <Particle.hpp>




class ExampleLayer : public Engine::Layer {
    public:
    ExampleLayer() : Layer("Example") {}
    void OnUpdate() override {
        // CLIENT_INFO("ExampleLayer::Update");

        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        auto vulkandata = renderer->GetVulkanData();
        renderer->BeginRecord();

        if (Engine::Input::IsKeyPressed(GLFW_KEY_A)) {
            CLIENT_TRACE("A key is pressed");
            }
        }
    void OnEvent(Engine::Event& event) override {
        // CLIENT_INFO("{0}", event);
        }

    void OnAttach() override {
        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        // std::vector<Engine::Particle> particles;
        // int numParticles = 1000;
        // particles.reserve(numParticles);
        // for (int i = 0; i < numParticles; i++) {
        //     particles.emplace_back();
        //     }
        renderer->addModel("../../resources/models/viking_room.obj");
        }

    };

class Sandbox : public Engine::Application {
    public:
    Sandbox() {
        PushLayer(new Editor::ImGuiLayer());

        PushLayer(new ExampleLayer());
        // PushOverlay(new Editor::ImGuiLayer());
        }
    ~Sandbox() {}
    };
extern Engine::Application* Engine::CreateApplication();
Engine::Application* Engine::CreateApplication() { return new Sandbox(); }