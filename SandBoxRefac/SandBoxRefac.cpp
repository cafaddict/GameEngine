#include "Log.hpp"
#include "OpenGLRenderer.hpp"
#include "Rendering.hpp"
#include "VulkanRenderer.hpp"
#include <memory>
#include <string>
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
        m_AssetManager->SetGraphicsAPI(Engine::GraphicsAPI::Vulkan);
        m_EntityManager = std::make_shared<Engine::EntityManager>();
    }
    ExampleLayer(std::shared_ptr<Engine::EntityManager> entityManager,
                 std::shared_ptr<Engine::AssetManager> assetManager)
        : Layer("Example"), m_AssetManager(assetManager), m_EntityManager(entityManager) {}
    void OnUpdate() override {
        Engine::Application &app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer *>(app.GetRenderer());
        renderer->Draw();
    }
    void OnEvent(Engine::Event &event) override {
        // CLIENT_INFO("{0}", event);
    }

    void OnAttach() override {
        Engine::Application &app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer *>(app.GetRenderer());

        renderer->SetEntityManager(m_EntityManager);

        std::string modelPah =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Raptor_Animated_FBX.fbx";
        auto modelData = m_AssetManager->GetAsset<Engine::ModelData>(modelPah);

        std::vector<std::shared_ptr<Engine::ModelComponent>> modelComponents;
        for (auto mesh : modelData->meshes) {
            auto modelComponent = std::make_shared<Engine::ModelComponent>(mesh);
            modelComponents.push_back(modelComponent);
        }
        // auto modelComponent = std::make_shared<Engine::ModelComponent>(modelData);

        std::string vertexShaderPath = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/shaders/vert_pbr450.spv";
        std::string fragmentShaderPath =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/shaders/frag_pbr450.spv";
        auto vertexShaderData = m_AssetManager->GetAsset<Engine::VertexShaderData>(vertexShaderPath);
        auto fragmentShaderData = m_AssetManager->GetAsset<Engine::FragmentShaderData>(fragmentShaderPath);
        auto shaderComponent = std::make_shared<Engine::ShaderComponent>(vertexShaderData, fragmentShaderData, nullptr);

        auto transformComponent = std::make_shared<Engine::TransformComponent>();
        transformComponent->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        // transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
        transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, 90.0f, 0.0f)));
        transformComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        auto entity1 = m_EntityManager->CreateEntity("entity1");
        entity1->AddComponent(modelComponents[0]);

        auto entity2 = m_EntityManager->CreateEntity("entity2");
        entity2->AddComponent(modelComponents[1]);

        std::string texturePathAlbedo =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Textures/Raptor_PatternA_Albedo.png";
        std::string texturePathNormal =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Textures/Raptor_Normal.png";
        std::string textureAO =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Textures/Raptor_AO.png";

        std::string texturePathOtherAlbedo =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Textures/Other_Albedo.png";
        std::string texturePathOtherNormal =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Textures/Other_Normal 1k.png";
        std::string textureOtherAO =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Raptor/Textures/Raptor_AO.png";

        auto textureDataAlbedo = m_AssetManager->GetAsset<Engine::TextureData>(texturePathAlbedo);
        auto textureComponentAlbedo = std::make_shared<Engine::TextureComponent>(textureDataAlbedo);
        entity1->AddComponent(textureComponentAlbedo);

        auto textureDataNormal = m_AssetManager->GetAsset<Engine::TextureData>(texturePathNormal);
        auto textureComponentNormal = std::make_shared<Engine::TextureComponent>(textureDataNormal);
        entity1->AddComponent(textureComponentNormal);

        auto textureDataAO = m_AssetManager->GetAsset<Engine::TextureData>(textureAO);
        auto textureComponentAO = std::make_shared<Engine::TextureComponent>(textureDataAO);
        entity1->AddComponent(textureComponentAO);

        auto textureDataOtherAlbedo = m_AssetManager->GetAsset<Engine::TextureData>(texturePathOtherAlbedo);
        auto textureComponentOtherAlbedo = std::make_shared<Engine::TextureComponent>(textureDataOtherAlbedo);
        entity2->AddComponent(textureComponentOtherAlbedo);

        auto textureDataOtherNormal = m_AssetManager->GetAsset<Engine::TextureData>(texturePathOtherNormal);
        auto textureComponentOtherNormal = std::make_shared<Engine::TextureComponent>(textureDataOtherNormal);
        entity2->AddComponent(textureComponentOtherNormal);

        auto textureDataOtherAO = m_AssetManager->GetAsset<Engine::TextureData>(textureOtherAO);
        auto textureComponentOtherAO = std::make_shared<Engine::TextureComponent>(textureDataOtherAO);
        entity2->AddComponent(textureComponentOtherAO);

        entity1->AddComponent(shaderComponent);
        entity1->AddComponent(transformComponent);

        entity2->AddComponent(shaderComponent);
        // auto transformComponent2 = std::make_shared<Engine::TransformComponent>();

        entity2->AddComponent(transformComponent);

        renderer->SetEntityUpdate(true);
    }
};

class SandboxRefac : public Engine::Application {
    public:
    SandboxRefac() {
        SetWindow(Engine::RendererType::Vulkan);
        SetRenderer(Engine::RendererType::Vulkan);
        ENGINE_WARN("SandboxRefac");
        auto EntityManager = std::make_shared<Engine::EntityManager>();
        auto AssetManager = std::make_shared<Engine::AssetManager>();
        AssetManager->SetGraphicsAPI(Engine::GraphicsAPI::Vulkan);
        PushLayer(new Editor::ImGuiLayer(EntityManager, AssetManager));
        PushLayer(new ExampleLayer(EntityManager, AssetManager));
        // PushOverlay(new Editor::ImGuiLayer());
    }
    ~SandboxRefac() {}
};
extern Engine::Application *Engine::CreateApplication();
Engine::Application *Engine::CreateApplication() { return new SandboxRefac(); }
