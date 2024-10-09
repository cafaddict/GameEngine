#include "OpenGLRenderer.hpp"
#include "Log.hpp"
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

} // namespace Engine