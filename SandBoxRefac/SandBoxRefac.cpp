#include "VulkanRenderer.hpp"
#define GLFW_INCLUDE_NONE
#include <AssetManager.hpp>
#include <Engine.hpp>
#include <EntityManager.hpp>
#include <ModelComponent.hpp>
#include <ModelData.hpp>
#include <ShaderComponent.hpp>
#include <ShaderData.hpp>
#include <TextureComponent.hpp>
#include <TransformComponent.hpp>
// #include <ImGuiLayer.hpp>
// #include <Input.hpp>
// #include <Application.hpp>

class ExampleLayer : public Engine::Layer {
    public:
    std::shared_ptr<Engine::AssetManager> m_AssetManager;
    std::shared_ptr<Engine::EntityManager> m_EntityManager;
    ExampleLayer() : Layer("Example") {
        m_AssetManager = std::make_shared<Engine::AssetManager>();
        m_EntityManager = std::make_shared<Engine::EntityManager>();
    }
    ExampleLayer(std::shared_ptr<Engine::EntityManager> entityManager,
                 std::shared_ptr<Engine::AssetManager> assetManager)
        : Layer("Example"), m_EntityManager(entityManager), m_AssetManager(assetManager) {}
    void OnUpdate() override {}
    void OnEvent(Engine::Event &event) override {
        // CLIENT_INFO("{0}", event);
    }

    void OnAttach() override {
        Engine::Application &app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer_refac *>(app.GetRenderer());
        // renderer->SetEntityManager(m_EntityManager);
    }
};

class Sandbox : public Engine::Application {
    public:
    Sandbox() {
        auto EntityManager = std::make_shared<Engine::EntityManager>();
        auto AssetManager = std::make_shared<Engine::AssetManager>();

        PushLayer(new ExampleLayer(EntityManager, AssetManager));
        // PushOverlay(new Editor::ImGuiLayer());
    }
    ~Sandbox() {}
};
extern Engine::Application *Engine::CreateApplication();
Engine::Application *Engine::CreateApplication() { return new Sandbox(); }
