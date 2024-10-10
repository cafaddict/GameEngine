#pragma once

#include <functional>
#include <string>

#include "Event.hpp"
#include "Rendering.hpp"

namespace Engine {
struct WindowProps {
    std::string Title;
    unsigned int Width;
    unsigned int Height;
    RendererType Renderer;

    WindowProps(const std::string &title = "Initial Application", unsigned int width = 1200, unsigned int height = 800,
                RendererType renderer = RendererType::Vulkan)
        : Title(title), Width(width), Height(height), Renderer(renderer) {}
};

// Interface representing a desktop system based Window
class Window {
    public:
    using EventCallbackFn = std::function<void(Event &)>;
    virtual ~Window() {}
    virtual void OnUpdate() = 0;
    virtual void OnWait() = 0;
    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    virtual void SetEventCallback(const EventCallbackFn &callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    virtual void *GetNativeWindow() const = 0;

    static Window *Create(const WindowProps &props = WindowProps());
};
} // namespace Engine