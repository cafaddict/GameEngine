#include "OpenGLRenderer.hpp"
#include "Log.hpp"
#include "ShaderComponent.hpp"
#include "TextureComponent.hpp"
#include "ShaderComponent.hpp"
#include "ModelComponent.hpp"
#include "TransformComponent.hpp"
#include "OpenGLTexture.hpp"
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
void OpenGLRenderer::Draw() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto entities = m_EntityManager->GetAllEntities();
    for (auto &entity : entities) {
        auto program = m_EntityPrograms[entity];
        auto mesh = m_EntityMeshes[entity];
        auto texture = m_EntityTextures[entity];
    }
    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}
void OpenGLRenderer::BeginRecord() {}
void OpenGLRenderer::EndRecord() {
    if (m_EntityUpdate) {
        createEntityResources();
        m_EntityUpdate = false;
    }
}
void OpenGLRenderer::SetWindow(GLFWwindow *window) { m_Window = window; }
void OpenGLRenderer::SetWindowResized(bool resized) { m_Resized = resized; }
void OpenGLRenderer::SetWindowMinimized(bool minimized) { m_Minimizied = minimized; }
bool OpenGLRenderer::GetEntityUpdate() { return m_EntityUpdate; }
void OpenGLRenderer::SetEntityUpdate(bool update) { m_EntityUpdate = update; }
void OpenGLRenderer::WaitIdle() {}

void OpenGLRenderer::createEntityResources() {

    auto entities = m_EntityManager->GetAllEntities();
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::mat4> transformations;

    for (auto &entity : entities) {
        auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();

        auto texture_data = entity->GetComponent<TextureComponent>()->GetTextureData();

        auto vertex_shader_data = entity->GetComponent<ShaderComponent>()->GetVertexShader();

        auto fragment_shader_data = entity->GetComponent<ShaderComponent>()->GetFragmentShader();

        auto compute_shader_data = entity->GetComponent<ShaderComponent>()->GetComputeShader();

        auto transform = entity->GetComponent<TransformComponent>()->GetTransformMatrix();

        auto vertexShaderCode =
            vertex_shader_data ? std::make_optional(vertex_shader_data->GetShaderCode()) : std::nullopt;
        auto fragmentShaderCode =
            fragment_shader_data ? std::make_optional(fragment_shader_data->GetShaderCode()) : std::nullopt;
        auto computeShaderCode =
            compute_shader_data ? std::make_optional(compute_shader_data->GetShaderCode()) : std::nullopt;

        if (computeShaderCode) {
            std::shared_ptr<OpenGLProgram> program = std::make_shared<OpenGLProgram>(
                vertexShaderCode.value(), fragmentShaderCode.value(), computeShaderCode.value());
            m_EntityPrograms[entity] = program;

        } else {
            std::shared_ptr<OpenGLProgram> program =
                std::make_shared<OpenGLProgram>(vertexShaderCode.value(), fragmentShaderCode.value());
            m_EntityPrograms[entity] = program;
        }

        ENGINE_TRACE("model_data->positions.size() = {0}", model_data->positions.size());
        ENGINE_TRACE("model_data->normals.size() = {0}", model_data->normals.size());
        ENGINE_TRACE("model_data->uvs.size() = {0}", model_data->uvs.size());

        std::shared_ptr<OpenGLMesh> mesh = std::make_shared<OpenGLMesh>(model_data);
        m_EntityMeshes[entity] = mesh;

        std::shared_ptr<OpenGLTexture> texture = std::make_shared<OpenGLTexture>(texture_data);
        m_EntityTextures[entity] = texture;
    }
}

} // namespace Engine