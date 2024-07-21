#define GLFW_INCLUDE_NONE
#include <Engine.hpp>
#include <AssetManager.hpp>
#include <ModelData.hpp>
#include <ShaderData.hpp>
#include <EntityManager.hpp>
#include <ModelComponent.hpp>
#include <ShaderComponent.hpp>
#include <TransformComponent.hpp>
#include <TextureComponent.hpp>
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
    ExampleLayer(std::shared_ptr<Engine::EntityManager> entityManager) : Layer("Example"), m_EntityManager(entityManager) {
        m_AssetManager = std::make_shared<Engine::AssetManager>();
        }
    void OnUpdate() override {
        // CLIENT_INFO("ExampleLayer::Update");

        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        auto vulkandata = renderer->GetVulkanData();
        auto entity1 = m_EntityManager->GetEntity("entity1");
        auto transform1 = entity1->GetComponent<Engine::TransformComponent>();
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(
            currentTime - startTime)
            .count();
        // transform1->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, time * glm::radians(90.0f))));

        renderer->BeginRecord();

        // if (Engine::Input::IsKeyPressed(GLFW_KEY_A)) {
        //     CLIENT_TRACE("A key is pressed");
        //     }
        }
    void OnEvent(Engine::Event& event) override {
        // CLIENT_INFO("{0}", event);
        }

    void OnAttach() override {
        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        renderer->SetEntityManager(m_EntityManager);

        std::string modelPah = "../../resources/models/viking_room.obj";
        auto modelData = m_AssetManager->GetAsset<Engine::ModelData>(modelPah);
        if (modelData) {
            ENGINE_INFO("Model loaded");
            }
        else {
            ENGINE_ERROR("Model load failed");
            }

        std::string texturePath = "../../resources/models/viking_room.png";
        auto textureData = m_AssetManager->GetAsset<Engine::TextureData>(texturePath);

        std::string vertexShaderPath = "../../resources/shaders/vert.spv";
        std::string fragmentShaderPath = "../../resources/shaders/frag.spv";

        auto vertexShaderData = m_AssetManager->GetAsset<Engine::VertexShaderData>(vertexShaderPath);
        auto fragmentShaderData = m_AssetManager->GetAsset<Engine::FragmentShaderData>(fragmentShaderPath);




        auto transformComponent = std::make_shared<Engine::TransformComponent>();
        transformComponent->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
        // transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
        transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
        transformComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        auto modelComponent = std::make_shared<Engine::ModelComponent>(modelData);
        auto textureComponent = std::make_shared<Engine::TextureComponent>(textureData);
        auto shaderComponent = std::make_shared <Engine::ShaderComponent>(vertexShaderData, fragmentShaderData, nullptr);

        auto entity1 = m_EntityManager->CreateEntity("entity1");
        entity1->AddComponent(modelComponent);
        entity1->AddComponent(textureComponent);
        entity1->AddComponent(shaderComponent);
        entity1->AddComponent(transformComponent);


        auto transformComponent2 = std::make_shared<Engine::TransformComponent>();
        transformComponent2->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        transformComponent2->SetRotation(glm::quat(glm::vec3(0.0f, glm::radians(45.0f), 0.0f)));
        // transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
        transformComponent2->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        auto entity2 = m_EntityManager->CreateEntity("entity2");
        entity2->AddComponent(modelComponent);
        entity2->AddComponent(textureComponent);
        entity2->AddComponent(shaderComponent);
        entity2->AddComponent(transformComponent2);







        // renderer->loadModel();
        // renderer->createGraphicsPipeline();
        renderer->createEntityResources();
        // renderer->createTextureImage();

        // renderer->addModel("../../resources/models/viking_room.obj");

        }
    };

class Sandbox : public Engine::Application {
    public:
    Sandbox() {
        auto EntityManager = std::make_shared<Engine::EntityManager>();
        PushLayer(new Editor::ImGuiLayer(EntityManager));

        PushLayer(new ExampleLayer(EntityManager));
        // PushOverlay(new Editor::ImGuiLayer());
        }
    ~Sandbox() {}
    };
extern Engine::Application* Engine::CreateApplication();
Engine::Application* Engine::CreateApplication() { return new Sandbox(); }