#include "OpenGLRenderer.hpp"
#include "Log.hpp"
#include "ShaderComponent.hpp"
#include "TextureComponent.hpp"
#include "ShaderComponent.hpp"
#include "ModelComponent.hpp"
#include "TransformComponent.hpp"
namespace Engine {
// Renderer *Renderer::Create(GLFWwindow *window) {
//     ENGINE_INFO("Vulkan Renderer Creation");
//     return new OpenGLRenderer(window);
// }
OpenGLRenderer *OpenGLRenderer::Create(GLFWwindow *window) { return new OpenGLRenderer(window); }
OpenGLRenderer::OpenGLRenderer() {}
OpenGLRenderer::OpenGLRenderer(GLFWwindow *window) {
    SetWindow(window);
    Init();
}
OpenGLRenderer::~OpenGLRenderer() {}
void OpenGLRenderer::Init() {
    ENGINE_INFO("OpenGL Renderer Init");
    glfwMakeContextCurrent(m_Window);
    // Initialize OpenGL context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        ENGINE_ERROR("Failed to initialize GLAD");
    }
    ENGINE_INFO("OpenGL Version: {0}", glGetString(GL_VERSION));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    int width;
    int height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    glViewport(0, 0, width, height);
}
void OpenGLRenderer::Draw() {}
void OpenGLRenderer::BeginRecord() {}
void OpenGLRenderer::EndRecord() {}
void OpenGLRenderer::SetWindow(GLFWwindow *window) { m_Window = window; }
void OpenGLRenderer::SetWindowResized(bool resized) { m_Resized = resized; }
void OpenGLRenderer::SetWindowMinimized(bool minimized) { m_Minimizied = minimized; }
bool OpenGLRenderer::GetEntityUpdate() { return m_EntityUpdate; }
void OpenGLRenderer::SetEntityUpdate(bool update) { m_EntityUpdate = update; }
void OpenGLRenderer::WaitIdle() {}

void OpenGLRenderer::createEntityResources() {
    uint32_t currentVertexOffset = 0;
    uint32_t currentIndexOffset = 0;

    auto entities = m_EntityManager->GetAllEntities();
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::mat4> transformations;
    std::unordered_map<std::shared_ptr<Entity>, size_t> vertexOffsets;
    std::unordered_map<std::shared_ptr<Entity>, size_t> indexOffsets;

    for (auto &entity : entities) {
        auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();

        auto texture_data = entity->GetComponent<TextureComponent>()->GetTextureData();

        auto vertex_shader_data = entity->GetComponent<ShaderComponent>()->GetVertexShader();

        auto fragment_shader_data = entity->GetComponent<ShaderComponent>()->GetFragmentShader();

        auto compute_shader_data = entity->GetComponent<ShaderComponent>()->GetComputeShader();

        auto transform = entity->GetComponent<TransformComponent>()->GetTransformMatrix();

        size_t vertexCount = model_data->positions.size();
        size_t indexCount = model_data->indices.size();

        ENGINE_TRACE("model_data->positions.size() = {0}", model_data->positions.size());
        ENGINE_TRACE("model_data->normals.size() = {0}", model_data->normals.size());
        ENGINE_TRACE("model_data->uvs.size() = {0}", model_data->uvs.size());

        for (size_t i = 0; i < vertexCount; i++) {
            Vertex vertex;
            vertex.pos = model_data->positions[i];
            vertex.normal = model_data->normals.size() > i ? model_data->normals[i] : glm::vec3(0.0f, 0.0f, 1.0f);
            vertex.texCoord = model_data->uvs.size() > i ? model_data->uvs[i] : glm::vec2(0.0f);
            vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
            vertices.push_back(vertex);
        }
    }
}

} // namespace Engine