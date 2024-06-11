#include "ImGuiLayer.hpp"
#include "imgui_impl_vulkan.cpp"

namespace Editor
    {
#define BIND_EVENT_FN(x) std::bind(&ImGuiLayer::x, this, std::placeholders::_1)
    ImGuiLayer::ImGuiLayer() : Engine::Layer("ImGui Layer") {}
    ImGuiLayer::~ImGuiLayer() {}
    void ImGuiLayer::OnAttach()
        {
        ENGINE_WARN("ImGuiLayer");
        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        auto vulkandata = renderer->GetVulkanData();
        auto window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
        // 1: create descriptor pool for IMGUI
        //  the size of the pool is very oversize, but it's copied from imgui demo
        //  itself.
        VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        ENGINE_WARN("CreateDescriptorPool");
        vkCreateDescriptorPool(vulkandata.device, &pool_info, nullptr, &imguiPool);
        // 2: initialize imgui library

        // this initializes the core structures of imgui
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

        ImGui::StyleColorsDark();

        // this initializes imgui for SDL
        ENGINE_WARN("InitForGLFW_Vulkan");
        ImGui_ImplGlfw_InitForVulkan(window, true);

        // this initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vulkandata.instance;
        init_info.PhysicalDevice = vulkandata.physicalDevice;
        init_info.Device = vulkandata.device;
        init_info.Queue = vulkandata.graphicsQueue;
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = vulkandata.msaaSamples;
        init_info.PipelineCache = g_PipelineCache;
        ENGINE_WARN("Vulkan_Init");
        ImGui_ImplVulkan_Init(&init_info, vulkandata.renderPass);


        VkCommandBuffer cmd = vulkandata.commandBuffers[vulkandata.currentFrame];
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        // vkResetCommandBuffer(vulkandata.commandBuffers[vulkandata.currentFrame],
        //                      /*VkCommandBufferResetFlagBits*/ 0);


        vkBeginCommandBuffer(cmd, &beginInfo);
        ENGINE_WARN("ImGui_ImplVulakn");
        ImGui_ImplVulkan_CreateFontsTexture(cmd);
        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &cmd;

        vkEndCommandBuffer(cmd);
        vkQueueSubmit(vulkandata.graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
        vkDeviceWaitIdle(vulkandata.device);

        // VkSubmitInfo submitInfo{};
        // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // VkSemaphore waitSemaphores[] = {
        //     vulkandata.imageAvailableSemaphores[vulkandata.currentFrame]};
        // VkPipelineStageFlags waitStages[] = {
        //     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        // submitInfo.waitSemaphoreCount = 1;
        // submitInfo.pWaitSemaphores = waitSemaphores;
        // submitInfo.pWaitDstStageMask = waitStages;

        // submitInfo.commandBufferCount = 1;
        // submitInfo.pCommandBuffers =
        //     &vulkandata.commandBuffers[vulkandata.currentFrame];

        // VkSemaphore signalSemaphores[] = {
        //     vulkandata.renderFinishedSemaphores[vulkandata.currentFrame]};
        // submitInfo.signalSemaphoreCount = 1;
        // submitInfo.pSignalSemaphores = signalSemaphores;
        // ENGINE_WARN("Queue submition");
        // vkResetFences(vulkandata.device, 1,
        //               &vulkandata.inFlightFences[vulkandata.currentFrame]);
        // if (vkQueueSubmit(vulkandata.graphicsQueue, 1, &submitInfo,
        //                   vulkandata.inFlightFences[vulkandata.currentFrame]) !=
        //     VK_SUCCESS) {
        //   ENGINE_ERROR("queue submition failed");
        //   throw std::runtime_error("failed to submit draw command buffer!");
        // };
        // vkResetFences(vulkandata.device, 1,
        //               &vulkandata.inFlightFences[vulkandata.currentFrame]);
        // vkWaitForFences(vulkandata.device, 1,
        //                 &vulkandata.inFlightFences[vulkandata.currentFrame], true,
        //                 9999999999);
        // vkResetFences(vulkandata.device, 1,
        //               &vulkandata.inFlightFences[vulkandata.currentFrame]);

        // // reset the command buffers inside the command pool
        // vkResetCommandPool(vulkandata.device, vulkandata.commandPool, 0);

        // clear font textures from cpu data

        ImGui_ImplVulkan_DestroyFontUploadObjects();
        ENGINE_WARN("ImGui Attach Finished");
        }
    void ImGuiLayer::OnDetach()
        {
        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        auto vulkandata = renderer->GetVulkanData();
        vkDestroyDescriptorPool(vulkandata.device, imguiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        }
    void ImGuiLayer::OnUpdate()
        {
        ImGuiIO& io = ImGui::GetIO();
        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        auto vulkandata = renderer->GetVulkanData();
        auto window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
        io.DisplaySize =
            ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

        float time = (float)glfwGetTime();
        io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60);
        m_Time = time;

        // ENGINE_WARN("Creating NewFrame");
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool show = true;
        ImGui::ShowDemoWindow(&show);

        // Rendering
        ImGui::Render();
        // ENGINE_WARN("Rendering data");
        ImDrawData* draw_data = ImGui::GetDrawData();
        FrameRender(draw_data);
        }
    void ImGuiLayer::FrameRender(ImDrawData* draw_data)
        {
        Engine::Application& app = Engine::Application::Get();
        auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
        auto vulkandata = renderer->GetVulkanData();
        auto window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
        VkResult err;

        // VkSemaphore image_acquired_semaphore =
        //     vulkandata.imageAvailableSemaphores[vulkandata.currentFrame];
        // VkSemaphore render_complete_semaphore =
        //     vulkandata.renderFinishedSemaphores[vulkandata.currentFrame];
        // uint32_t imageIndex;
        // err = vkAcquireNextImageKHR(vulkandata.device, vulkandata.swapChain,
        //     UINT64_MAX, image_acquired_semaphore,
        //     VK_NULL_HANDLE, &imageIndex);
        // if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
        //     {
        //     renderer->recreateSwapChain();
        //     return;
        //     }

        // {
        // err = vkWaitForFences(
        //     vulkandata.device, 1,
        //     &vulkandata.inFlightFences[vulkandata.currentFrame], VK_TRUE,
        //     UINT64_MAX); // wait indefinitely instead of periodically checking

        // err = vkResetFences(vulkandata.device, 1,
        //     &vulkandata.inFlightFences[vulkandata.currentFrame]);
        // }
        // {
        // // err = vkResetCommandPool(vulkandata.device, vulkandata.commandPool, 0);

        // VkCommandBufferBeginInfo info = {};
        // info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        // err = vkBeginCommandBuffer(
        //     vulkandata.commandBuffers[vulkandata.currentFrame], &info);
        // }
        {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = vulkandata.renderPass;
        // info.framebuffer = vulkandata.swapChainFramebuffers[imageIndex];
        info.framebuffer = vulkandata.swapChainGUIFramebuffers[vulkandata.image_index];

        info.renderArea.extent.width = app.GetWindow().GetWidth();
        info.renderArea.extent.height = app.GetWindow().GetHeight();
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };
        info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        info.pClearValues = clearValues.data();


        ImGui_ImplVulkan_Data* gui_vulkandata = ImGui_ImplVulkan_GetBackendData();
        vkCmdBindPipeline(vulkandata.commandBuffers[vulkandata.currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, gui_vulkandata->Pipeline);


        // vkCmdBeginRenderPass(vulkandata.commandBuffers[vulkandata.currentFrame],
        //     &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(
            draw_data, vulkandata.commandBuffers[vulkandata.currentFrame]);

        // Submit command buffer
        vkCmdEndRenderPass(vulkandata.commandBuffers[vulkandata.currentFrame]);
        // {
        // VkPipelineStageFlags wait_stage =
        //     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        // VkSubmitInfo info = {};
        // info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        // info.waitSemaphoreCount = 1;
        // info.pWaitSemaphores = &image_acquired_semaphore;
        // info.pWaitDstStageMask = &wait_stage;
        // info.commandBufferCount = 1;
        // info.pCommandBuffers = &vulkandata.commandBuffers[vulkandata.currentFrame];
        // info.signalSemaphoreCount = 1;
        // info.pSignalSemaphores = &render_complete_semaphore;

        // vkEndCommandBuffer(vulkandata.commandBuffers[vulkandata.currentFrame]);

        // vkQueueSubmit(vulkandata.graphicsQueue, 1, &info,
        //     vulkandata.inFlightFences[vulkandata.currentFrame]);
        // }

        // VkPresentInfoKHR info = {};
        // info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        // info.waitSemaphoreCount = 1;
        // info.pWaitSemaphores = &render_complete_semaphore;

        // VkSwapchainKHR swapChains[] = {vulkandata.swapChain};
        // info.swapchainCount = 1;
        // info.pSwapchains = swapChains;
        // info.pImageIndices = &imageIndex;
        // auto result = vkQueuePresentKHR(vulkandata.presentQueue, &info);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        //     vulkandata.framebufferResized)
        // {
        //     vulkandata.framebufferResized = false;
        //     renderer->recreateSwapChain();
        // }
        // else if (result != VK_SUCCESS)
        // {
        //     throw std::runtime_error("failed to present swap chain image!");
        // }

        // vulkandata.currentFrame = (vulkandata.currentFrame + 1) % 2;
        }

    void ImGuiLayer::OnWait() {}
    void ImGuiLayer::OnEvent(Engine::Event& event)
        {
        Engine::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
            BIND_EVENT_FN(OnMouseButtonPressedEvent));
        dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
            BIND_EVENT_FN(OnMouseButtonReleasedEvent));
        dispatcher.Dispatch<Engine::MouseMovedEvent>(
            BIND_EVENT_FN(OnMouseMovedEvent));
        dispatcher.Dispatch<Engine::MouseScrolledEvent>(
            BIND_EVENT_FN(OnMouseScrolledEvent));
        dispatcher.Dispatch<Engine::KeyPressedEvent>(
            BIND_EVENT_FN(OnKeyPressedEvent));
        dispatcher.Dispatch<Engine::KeyReleasedEvent>(
            BIND_EVENT_FN(OnKeyReleasedEvent));
        dispatcher.Dispatch<Engine::WindowResizeEvent>(
            BIND_EVENT_FN(OnWindowResizeEvent));
        dispatcher.Dispatch<Engine::KeyTypedEvent>(BIND_EVENT_FN(OnKeyTypedEvent));
        }

    bool ImGuiLayer::OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(e.GetMouseButton(), true);

        return false;
        }
    bool ImGuiLayer::OnMouseButtonReleasedEvent(
        Engine::MouseButtonReleasedEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(e.GetMouseButton(), false);

        return false;
        }
    bool ImGuiLayer::OnMouseMovedEvent(Engine::MouseMovedEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMousePosEvent(e.GetX(), e.GetY());

        return false;
        }
    bool ImGuiLayer::OnMouseScrolledEvent(Engine::MouseScrolledEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());

        return false;
        }
    bool ImGuiLayer::OnKeyPressedEvent(Engine::KeyPressedEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        int mods = e.GetMods();
        if (int keycode_to_mod = ImGui_ImplGlfw_KeyToModifier(e.GetKeyCode()))
            mods = (mods | keycode_to_mod);

        ImGui_ImplGlfw_UpdateKeyModifiers(mods);
        ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
        io.AddKeyEvent(imgui_key, true);

        return false;
        }
    bool ImGuiLayer::OnKeyReleasedEvent(Engine::KeyReleasedEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
        io.AddKeyEvent(imgui_key, false);

        return false;
        }
    bool ImGuiLayer::OnWindowResizeEvent(Engine::WindowResizeEvent& e)
        {
        return false;
        }

    bool ImGuiLayer::OnKeyTypedEvent(Engine::KeyTypedEvent& e)
        {
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(e.GetCharacter());
        return false;
        }

    } // namespace Editor