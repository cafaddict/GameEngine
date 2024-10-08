#include "VulkanRenderer.hpp"
#include <memory>
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
    void OnUpdate() override {
        Engine::Application &app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer_refac *>(app.GetRenderer());
        renderer->Draw();
    }
    void OnEvent(Engine::Event &event) override {
        // CLIENT_INFO("{0}", event);
    }

    void OnAttach() override {
        Engine::Application &app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer_refac *>(app.GetRenderer());

        renderer->SetEntityManager(m_EntityManager);

        std::string modelPah = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/viking_room.obj";
        auto modelData = m_AssetManager->GetAsset<Engine::ModelData>(modelPah);
        auto modelComponent = std::make_shared<Engine::ModelComponent>(modelData);

        std::string texturePath = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/viking_room.png";
        auto textureData = m_AssetManager->GetAsset<Engine::TextureData>(texturePath);
        auto textureComponent = std::make_shared<Engine::TextureComponent>(textureData);

        std::string vertexShaderPath = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/shaders/vert.spv";
        std::string fragmentShaderPath = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/shaders/frag.spv";
        auto vertexShaderData = m_AssetManager->GetAsset<Engine::VertexShaderData>(vertexShaderPath);
        auto fragmentShaderData = m_AssetManager->GetAsset<Engine::FragmentShaderData>(fragmentShaderPath);
        auto shaderComponent = std::make_shared<Engine::ShaderComponent>(vertexShaderData, fragmentShaderData, nullptr);

        auto transformComponent = std::make_shared<Engine::TransformComponent>();
        transformComponent->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        // transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
        transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
        transformComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        auto entity1 = m_EntityManager->CreateEntity("entity1");
        entity1->AddComponent(modelComponent);
        entity1->AddComponent(textureComponent);
        entity1->AddComponent(shaderComponent);
        entity1->AddComponent(transformComponent);

        renderer->SetEntityUpdate(true);
    }
};

class SandboxRefac : public Engine::Application {
    public:
    SandboxRefac() {
        auto EntityManager = std::make_shared<Engine::EntityManager>();
        auto AssetManager = std::make_shared<Engine::AssetManager>();
        PushLayer(new Editor::ImGuiLayer(EntityManager, AssetManager));
        PushLayer(new ExampleLayer(EntityManager, AssetManager));
        // PushOverlay(new Editor::ImGuiLayer());
    }
    ~SandboxRefac() {}
};
extern Engine::Application *Engine::CreateApplication();
Engine::Application *Engine::CreateApplication() { return new SandboxRefac(); }
