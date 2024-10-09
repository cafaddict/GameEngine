#pragma once
#include "EntityManager.hpp"
#include "Rendering.hpp"
#include "Log.hpp"

namespace Engine {
class OpenGLRenderer : public Renderer {
    public:
    static OpenGLRenderer *Create(GLFWwindow *window);
    OpenGLRenderer();
    OpenGLRenderer(GLFWwindow *window);
    virtual ~OpenGLRenderer();
    virtual void Init() override;
    virtual void Draw() override;
    virtual void BeginRecord() override;
    virtual void EndRecord() override;
    virtual void SetWindow(GLFWwindow *window) override;
    virtual void SetWindowResized(bool resized) override;
    virtual void SetWindowMinimized(bool minimized) override;
    virtual bool GetEntityUpdate() override;
    virtual void SetEntityUpdate(bool update) override;
    virtual void WaitIdle() override;
    void SetEntityManager(std::shared_ptr<EntityManager> entitymanager) { m_EntityManager = entitymanager; }

    private:
    std::shared_ptr<EntityManager> m_EntityManager;
    GLFWwindow *m_Window = nullptr;
    bool m_Minimizied = false;
    bool m_Resized = false;
    bool m_EntityUpdate = false;
    std::vector<GLuint> m_VBOs;
};
} // namespace Engine