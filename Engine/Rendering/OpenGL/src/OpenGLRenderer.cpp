#include "OpenGLRenderer.hpp"
#include "Log.hpp"
#include "ShaderComponent.hpp"
#include "TextureComponent.hpp"
#include "ShaderComponent.hpp"
#include "ModelComponent.hpp"
#include "TransformComponent.hpp"
#include "OpenGLTexture.hpp"
#include "glm/fwd.hpp"
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
    // glEnable(GL_LESS);
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
    m_Camera = std::make_shared<Camera>(
        glm::lookAt(glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.01f, 1000.0f));

    m_Light = std::make_shared<Light>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
    ENGINE_INFO("OpenGL Renderer Initialized");
}

void OpenGLRenderer::Draw() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto entities = m_EntityManager->GetAllEntities();
    if (entities.empty() || m_EntityUpdate) {
        return;
    }
    for (auto &entity : entities) {
        auto program = m_EntityPrograms[entity];
        auto mesh = m_EntityMeshes[entity];
        auto texture = m_EntityTextures[entity];
        glUseProgram(program->GetProgram());

        GLuint cameraUBO, lightUBO, transformUBO;
        glGenBuffers(1, &cameraUBO);
        glGenBuffers(1, &lightUBO);
        glGenBuffers(1, &transformUBO);

        glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Camera), nullptr, GL_STATIC_DRAW);
        glm::mat4 viewMatrix = m_Camera->getView();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &viewMatrix);
        glm::mat4 projectionMatrix = m_Camera->getProjection();
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &projectionMatrix);

        glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(Light), nullptr, GL_STATIC_DRAW);
        glm::vec3 lightPosition = m_Light->getPosition();
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &lightPosition);
        glm::vec3 lightColor = m_Light->getColor();
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), sizeof(glm::vec3), &lightColor);
        float lightIntensity = m_Light->getIntensity();
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::vec3) * 2, sizeof(float), &lightIntensity);

        glBindBuffer(GL_UNIFORM_BUFFER, transformUBO);
        glBufferData(GL_UNIFORM_BUFFER, m_Transformations.size() * sizeof(glm::mat4), m_Transformations.data(),
                     GL_STATIC_DRAW);

        // Get the indices of each uniform block in the program
        GLuint cameraIndex = glGetUniformBlockIndex(program->GetProgram(), "CameraUBO");
        GLuint lightIndex = glGetUniformBlockIndex(program->GetProgram(), "LightUBO");
        GLuint transformIndex = glGetUniformBlockIndex(program->GetProgram(), "TransformUBO");

        // Bind the uniform blocks to binding points
        glUniformBlockBinding(program->GetProgram(), cameraIndex, 0);
        glUniformBlockBinding(program->GetProgram(), lightIndex, 1);
        glUniformBlockBinding(program->GetProgram(), transformIndex, 2);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightUBO);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, transformUBO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
        glUniform1i(glGetUniformLocation(program->GetProgram(), "texSampler"), 0);

        glBindVertexArray(mesh->GetVertexArray());
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
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

        transformations.push_back(transform);
    }
    m_Transformations = transformations;
}

} // namespace Engine