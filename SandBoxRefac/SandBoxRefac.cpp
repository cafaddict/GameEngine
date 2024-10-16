#include "Log.hpp"
#include "OpenGLRenderer.hpp"
#include "Rendering.hpp"
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

        std::string modelPah = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Cactus1.fbx";
        auto modelData = m_AssetManager->GetAsset<Engine::ModelData>(modelPah);
        auto modelComponent = std::make_shared<Engine::ModelComponent>(modelData);

        std::string vertexShaderPath =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/shaders/vert_vulkan_pbr.spv";
        std::string fragmentShaderPath =
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/shaders/frag_vulkan_pbr.spv";
        auto vertexShaderData = m_AssetManager->GetAsset<Engine::VertexShaderData>(vertexShaderPath);
        auto fragmentShaderData = m_AssetManager->GetAsset<Engine::FragmentShaderData>(fragmentShaderPath);
        auto shaderComponent = std::make_shared<Engine::ShaderComponent>(vertexShaderData, fragmentShaderData, nullptr);

        auto transformComponent = std::make_shared<Engine::TransformComponent>();
        transformComponent->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        // transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
        transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
        transformComponent->SetScale(glm::vec3(2.0f, 2.0f, 2.0f));

        auto entity1 = m_EntityManager->CreateEntity("entity1");
        entity1->AddComponent(modelComponent);

        if (modelData->texturePaths.empty()) {
            // std::string texturePath = "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/bunny-atlas.jpg";
            // auto textureData = m_AssetManager->GetAsset<Engine::TextureData>(texturePath);
            // auto textureComponent = std::make_shared<Engine::TextureComponent>(textureData);
            // entity1->AddComponent(textureComponent);
            std::string texturePath =
                "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Default_Base_Color.png";
            auto textureData = m_AssetManager->GetAsset<Engine::TextureData>(texturePath);
            auto textureComponent = std::make_shared<Engine::TextureComponent>(textureData);
            entity1->AddComponent(textureComponent);

            std::string texturePath2 =
                "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Default_Normal_OpenGL.png";
            auto textureData2 = m_AssetManager->GetAsset<Engine::TextureData>(texturePath2);
            auto textureComponent2 = std::make_shared<Engine::TextureComponent>(textureData2);
            entity1->AddComponent(textureComponent2);

            std::string texturePath3 =
                "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Default_Metallic.png";
            auto textureData3 = m_AssetManager->GetAsset<Engine::TextureData>(texturePath3);
            auto textureComponent3 = std::make_shared<Engine::TextureComponent>(textureData3);
            entity1->AddComponent(textureComponent3);

            std::string texturePath4 =
                "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Default_Roughness.png";
            auto textureData4 = m_AssetManager->GetAsset<Engine::TextureData>(texturePath4);
            auto textureComponent4 = std::make_shared<Engine::TextureComponent>(textureData4);
            entity1->AddComponent(textureComponent4);

            std::string texturePath5 =
                "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Default_Base_Color.png";
            "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/Default_Mixed_AO.png";
            auto textureData5 = m_AssetManager->GetAsset<Engine::TextureData>(texturePath5);
            auto textureComponent5 = std::make_shared<Engine::TextureComponent>(textureData5);
            entity1->AddComponent(textureComponent5);

        } else {
            ENGINE_WARN("Model contains {0} textures.", modelData->texturePaths.size());
            for (auto &texturePath : modelData->texturePaths) {

#ifndef _WIN32
                std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
#endif
                std::string trueTexturePath =
                    "/Users/hyunyul-cho/Documents/git/GameEngine/resources/models/Cactus/" + texturePath;
                auto textureData = m_AssetManager->GetAsset<Engine::TextureData>(trueTexturePath);
                auto textureComponent = std::make_shared<Engine::TextureComponent>(textureData);
                entity1->AddComponent(textureComponent);
            }
        }

        entity1->AddComponent(shaderComponent);
        entity1->AddComponent(transformComponent);

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
        // PushLayer(new Editor::ImGuiLayer(EntityManager, AssetManager));
        PushLayer(new ExampleLayer(EntityManager, AssetManager));
        // PushOverlay(new Editor::ImGuiLayer());
    }
    ~SandboxRefac() {}
};
extern Engine::Application *Engine::CreateApplication();
Engine::Application *Engine::CreateApplication() { return new SandboxRefac(); }
