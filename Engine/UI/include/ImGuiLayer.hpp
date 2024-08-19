#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_NONE
#include <imgui/imgui.h>

#include <Event.hpp>
#include <KeyEvent.hpp>
#include <MouseEvent.hpp>
#include <ApplicationEvent.hpp>
#include <Layer.hpp>
#include <Log.hpp>

// #include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Application.hpp>
#include <VulkanRendering.hpp>

#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

#include <EntityManager.hpp>
#include <AssetManager.hpp>
#include <memory>

#include <nfd.h>

namespace Editor
{
    class ImGuiLayer : public Engine::Layer
    {
    public:
        ImGuiLayer();
        ImGuiLayer(std::shared_ptr<Engine::EntityManager> entityMnager, std::shared_ptr<Engine::AssetManager> assetManager);
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnUpdate() override;
        virtual void OnWait() override;
        virtual void OnEvent(Engine::Event &event) override;

    private:
        std::shared_ptr<Engine::EntityManager> m_EntityManager;
        std::shared_ptr<Engine::AssetManager> m_AssetManager;
        float m_Time = 0.0f;
        VkDescriptorPool imguiPool;
        VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
        VkPipeline pipeline;

    private:
        bool OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent &e);
        bool OnMouseButtonReleasedEvent(Engine::MouseButtonReleasedEvent &e);
        bool OnMouseMovedEvent(Engine::MouseMovedEvent &e);
        bool OnMouseScrolledEvent(Engine::MouseScrolledEvent &e);
        bool OnKeyPressedEvent(Engine::KeyPressedEvent &e);
        bool OnKeyReleasedEvent(Engine::KeyReleasedEvent &e);
        bool OnWindowResizeEvent(Engine::WindowResizeEvent &e);
        bool OnKeyTypedEvent(Engine::KeyTypedEvent &e);
        void FrameRender(ImDrawData *draw_data);
        void FramePresent();

        // this is for model loading
    private:
        std::string selectedObjFilePath;
        std::string selectedTextureFilePath;
        char textBuffer[256] = "test";
    };
} // namespace Editor
