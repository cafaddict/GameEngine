#pragma once
#include <GLFW/glfw3.h>
#include <string>
// #include <Particle.hpp>
// #include <KHR/khrplatform.h>
// #include <glad/glad.h>

namespace Engine {
class Renderer {
    public:
    virtual ~Renderer() {}
    virtual void Init() = 0;
    virtual void Draw() = 0;
    virtual void addModel(std::string model_path) = 0;
    // virtual void addParticles(std::vector<Particle> particles) = 0;
    virtual void SetWindow(GLFWwindow *window) = 0;
    virtual void SetWindowResized(bool resized) = 0;
    virtual void SetWindowMinimized(bool minimized) = 0;
    static Renderer *Create();
    static Renderer *Create(GLFWwindow *window);

    // for vulkan
    virtual void WaitIdle() = 0;
};
} // namespace Engine