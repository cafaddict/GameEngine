#pragma once
#include "EntityManager.hpp"
#include "OpenGLTexture.hpp"
#include "Rendering.hpp"
#include "Log.hpp"
#include "Camera.hpp"
#include "OpenGLMesh.hpp"
#include "OpenGLProgram.hpp"
#include <unordered_map>

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
    void createEntityResources();

    private:
    std::shared_ptr<EntityManager> m_EntityManager;
    GLFWwindow *m_Window = nullptr;
    bool m_Minimizied = false;
    bool m_Resized = false;
    bool m_EntityUpdate = false;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<OpenGLMesh>> m_EntityMeshes;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<OpenGLProgram>> m_EntityPrograms;
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<OpenGLTexture>> m_EntityTextures;
};
} // namespace Engine