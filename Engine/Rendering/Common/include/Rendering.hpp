#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #include <Particle.hpp>
// #include <KHR/khrplatform.h>
// #include <glad/glad.h>

namespace Engine {
enum class RendererType { OpenGL, Vulkan };
class Renderer {
    public:
    virtual ~Renderer() {}
    virtual void Init() = 0;
    virtual void Draw() = 0;
    virtual void BeginRecord() = 0;
    virtual void EndRecord() = 0;

    // virtual void addParticles(std::vector<Particle> particles) = 0;
    virtual void SetWindow(GLFWwindow *window) = 0;
    virtual void SetWindowResized(bool resized) = 0;
    virtual void SetWindowMinimized(bool minimized) = 0;
    virtual bool GetEntityUpdate() = 0;
    virtual void SetEntityUpdate(bool update) = 0;
    static Renderer *Create();
    static Renderer *Create(GLFWwindow *window);
    static Renderer *Create(GLFWwindow *window, RendererType type);

    // for vulkan
    virtual void WaitIdle() = 0;
};
} // namespace Engine