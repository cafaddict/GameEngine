#include <ImGuiLayer.hpp>
#include "Log.hpp"
#include "VulkanRenderer.hpp"
#include "VulkanRendering.hpp"
#include "imgui.h"
#include "imgui_impl_vulkan.cpp"
#include <glm/gtc/type_ptr.hpp>

namespace Editor {
#define BIND_EVENT_FN(x) std::bind(&ImGuiLayer::x, this, std::placeholders::_1)
ImGuiLayer::ImGuiLayer() : Engine::Layer("ImGui Layer") {}
ImGuiLayer::ImGuiLayer(std::shared_ptr<Engine::EntityManager> entityManager,
                       std::shared_ptr<Engine::AssetManager> assetManager)
    : Engine::Layer("ImGui Layer"), m_EntityManager(entityManager), m_AssetManager(assetManager) {}
ImGuiLayer::~ImGuiLayer() {}

// void ImGuiLayer::OnAttach() {
//     ENGINE_WARN("ImGuiLayer");
//     Engine::Application &app = Engine::Application::Get();
//     auto renderer = static_cast<Engine::VulkanRenderer *>(app.GetRenderer());
//     auto vulkandata = renderer->GetVulkanData();
//     auto window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
//     // 1: create descriptor pool for IMGUI
//     //  the size of the pool is very oversize, but it's copied from imgui demo
//     //  itself.
//     VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
//                                          {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
//                                          {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
//                                          {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
//                                          {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
//                                          {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
//                                          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
//                                          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
//                                          {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
//                                          {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
//                                          {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

//     VkDescriptorPoolCreateInfo pool_info = {};
//     pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//     pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//     pool_info.maxSets = 1000;
//     pool_info.poolSizeCount = std::size(pool_sizes);
//     pool_info.pPoolSizes = pool_sizes;
//     ENGINE_WARN("CreateDescriptorPool");
//     vkCreateDescriptorPool(vulkandata.device, &pool_info, nullptr, &imguiPool);
//     // 2: initialize imgui library

//     // this initializes the core structures of imgui
//     ImGui::CreateContext();
//     ImGuiIO &io = ImGui::GetIO();
//     (void)io;
//     io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
//     io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

//     ImGui::StyleColorsDark();

//     // this initializes imgui for SDL
//     ENGINE_WARN("InitForGLFW_Vulkan");
//     ImGui_ImplGlfw_InitForVulkan(window, true);

//     // this initializes imgui for Vulkan
//     ImGui_ImplVulkan_InitInfo init_info = {};
//     init_info.Instance = vulkandata.instance;
//     init_info.PhysicalDevice = vulkandata.physicalDevice;
//     init_info.Device = vulkandata.device;
//     init_info.Queue = vulkandata.graphicsQueue;
//     init_info.DescriptorPool = imguiPool;
//     init_info.MinImageCount = 3;
//     init_info.ImageCount = 3;
//     init_info.MSAASamples = vulkandata.msaaSamples;
//     init_info.PipelineCache = g_PipelineCache;
//     ENGINE_WARN("Vulkan_Init");
//     ImGui_ImplVulkan_Init(&init_info, vulkandata.renderPass);

//     VkCommandBuffer cmd = vulkandata.commandBuffers[vulkandata.currentFrame];
//     VkCommandBufferBeginInfo beginInfo{};
//     beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//     beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

//     vkBeginCommandBuffer(cmd, &beginInfo);
//     ENGINE_WARN("ImGui_ImplVulakn");
//     ImGui_ImplVulkan_CreateFontsTexture(cmd);
//     VkSubmitInfo end_info = {};
//     end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//     end_info.commandBufferCount = 1;
//     end_info.pCommandBuffers = &cmd;

//     vkEndCommandBuffer(cmd);
//     vkQueueSubmit(vulkandata.graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
//     vkDeviceWaitIdle(vulkandata.device);

//     ImGui_ImplVulkan_DestroyFontUploadObjects();
//     ENGINE_WARN("ImGui Attach Finished");
// }

void ImGuiLayer::OnAttach() {
    ENGINE_WARN("ImGuiLayer");
    Engine::Application &app = Engine::Application::Get();
    auto renderer = static_cast<Engine::VulkanRenderer_refac *>(app.GetRenderer());
    auto window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    // 1: create descriptor pool for IMGUI
    //  the size of the pool is very oversize, but it's copied from imgui demo
    //  itself.
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    ENGINE_WARN("CreateDescriptorPool");
    vkCreateDescriptorPool(renderer->GetDevice()->getLogicalDevice(), &pool_info, nullptr, &imguiPool);
    // 2: initialize imgui library

    // this initializes the core structures of imgui
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    ImGui::StyleColorsDark();

    // this initializes imgui for SDL
    ENGINE_WARN("InitForGLFW_Vulkan");
    ImGui_ImplGlfw_InitForVulkan(window, true);

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = renderer->GetVulkanInstance()->getInstance();
    init_info.PhysicalDevice = renderer->GetDevice()->getPhysicalDevice();
    init_info.Device = renderer->GetDevice()->getLogicalDevice();
    init_info.Queue = renderer->GetDevice()->getGraphicsQueue();
    init_info.DescriptorPool = imguiPool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = renderer->GetDevice()->getMsaaSamples();
    init_info.PipelineCache = g_PipelineCache;
    ENGINE_WARN("Vulkan_Init");
    ImGui_ImplVulkan_Init(&init_info, renderer->GetRenderPass()->getRenderPass());

    VkCommandBuffer cmd = renderer->GetCommandBuffer()->getCommandBuffers()[renderer->GetCurrentFrame()];
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &beginInfo);
    ENGINE_WARN("ImGui_ImplVulakn");
    ImGui_ImplVulkan_CreateFontsTexture(cmd);
    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &cmd;

    vkEndCommandBuffer(cmd);
    vkQueueSubmit(renderer->GetDevice()->getGraphicsQueue(), 1, &end_info, VK_NULL_HANDLE);
    vkDeviceWaitIdle(renderer->GetDevice()->getLogicalDevice());

    ImGui_ImplVulkan_DestroyFontUploadObjects();
    ENGINE_WARN("ImGui Attach Finished");
}

void ImGuiLayer::OnDetach() {
    Engine::Application &app = Engine::Application::Get();
    auto renderer = static_cast<Engine::VulkanRenderer *>(app.GetRenderer());
    auto vulkandata = renderer->GetVulkanData();
    vkDestroyDescriptorPool(vulkandata.device, imguiPool, nullptr);
    ImGui_ImplVulkan_Shutdown();
}

struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

// Function to convert quaternion to Euler angles in degrees
glm::vec3 QuaternionToEuler(const glm::quat &q) {
    return glm::degrees(glm::eulerAngles(q)); // Convert radians to degrees
}

// Function to convert Euler angles in degrees to quaternion
glm::quat EulerToQuaternion(const glm::vec3 &euler) {
    return glm::quat(glm::radians(euler)); // Convert degrees to radians
}

void ImGuiLayer::OnUpdate() {

    ImGuiIO &io = ImGui::GetIO();
    Engine::Application &app = Engine::Application::Get();
    auto renderer = static_cast<Engine::VulkanRenderer_refac *>(app.GetRenderer());
    auto window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

    float time = (float)glfwGetTime();
    io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60);
    m_Time = time;

    // ENGINE_WARN("Creating NewFrame");
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    auto Entities = m_EntityManager->GetAllEntities();
    float startX = 100.0f;
    float startY = 100.0f;
    float windowWidth = 200.0f;
    float windowHeight = 100.0f;
    int index = 0;

    ImGui::Begin("OBJ File Loader");

    if (ImGui::Button("Load OBJ")) {
        // Native file dialog
        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);

        if (result == NFD_OKAY) {
            selectedObjFilePath = std::string(outPath);
            free(outPath); // Don't forget to free the allocated memory
            // Handle the file loading here
            ENGINE_INFO("Selected File: {0}", selectedObjFilePath.c_str());

            // Example: LoadOBJ(filePath); // Implement this function to handle the OBJ file
        } else if (result == NFD_CANCEL) {
            ENGINE_INFO("File dialog cancelled.");
        } else {
            ENGINE_ERROR("Error occurred while opening file dialog.");
        }
    }

    if (ImGui::Button("Load texture")) {
        // Native file dialog
        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_OpenDialog("png;jpg;jpeg", NULL, &outPath);

        if (result == NFD_OKAY) {
            selectedTextureFilePath = std::string(outPath);
            free(outPath); // Don't forget to free the allocated memory
            // Handle the file loading here
            ENGINE_INFO("Selected File: {0}", selectedTextureFilePath.c_str());

            // Example: LoadOBJ(filePath); // Implement this function to handle the OBJ file
        } else if (result == NFD_CANCEL) {
            ENGINE_INFO("File dialog cancelled.");
        } else {
            ENGINE_ERROR("Error occurred while opening file dialog.");
        }
    }

    ImGui::InputText("Entity name", textBuffer, sizeof(textBuffer));

    ImGui::Text("Selected Obj File: %s", selectedObjFilePath.c_str());
    ImGui::Text("Selected Texture File: %s", selectedTextureFilePath.c_str());
    ImGui::Text("Entity name: %s", textBuffer);

    if (ImGui::Button("Load Model")) {
        if (selectedObjFilePath.empty()) {
            ENGINE_INFO("Obj file is not selected");
        } else if (selectedTextureFilePath.empty()) {
            ENGINE_INFO("Texture file is not selected");
        } else {
            auto modelData = m_AssetManager->GetAsset<Engine::ModelData>(selectedObjFilePath);
            auto textureData = m_AssetManager->GetAsset<Engine::TextureData>(selectedTextureFilePath);
            std::string vertexShaderPath = "../../resources/shaders/vert.spv";
            std::string fragmentShaderPath = "../../resources/shaders/frag.spv";

            auto vertexShaderData = m_AssetManager->GetAsset<Engine::VertexShaderData>(vertexShaderPath);
            auto fragmentShaderData = m_AssetManager->GetAsset<Engine::FragmentShaderData>(fragmentShaderPath);
            auto transformComponent = std::make_shared<Engine::TransformComponent>();
            transformComponent->SetPosition(glm::vec3(2.0f, 0.0f, 0.0f));
            transformComponent->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
            transformComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

            auto modelComponent = std::make_shared<Engine::ModelComponent>(modelData);
            auto textureComponent = std::make_shared<Engine::TextureComponent>(textureData);
            auto shaderComponent =
                std::make_shared<Engine::ShaderComponent>(vertexShaderData, fragmentShaderData, nullptr);

            auto entity1 = m_EntityManager->CreateEntity(textBuffer);
            entity1->AddComponent(modelComponent);
            entity1->AddComponent(textureComponent);
            entity1->AddComponent(shaderComponent);
            entity1->AddComponent(transformComponent);
            renderer->SetEntityUpdate(true);
        }
    }

    ImGui::End();

    for (auto entity : Entities) {
        std::string entityID = entity->GetID();
        std::string title = "Panel : " + entityID;

        float posX = startX;
        float posY = startY + index * (windowHeight + 10); // 10 is the spacing between windows

        auto entityTransformComponent = entity->GetComponent<Engine::TransformComponent>();
        glm::vec3 entityPosition = entityTransformComponent->GetPosition();
        glm::quat entityRotation = entityTransformComponent->GetRotation();
        glm::vec3 entityScale = entityTransformComponent->GetScale();

        Transform transform;
        transform.position = entityPosition;
        transform.rotation = entityRotation;
        transform.scale = entityScale;

        ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_FirstUseEver);
        ImGui::Begin(title.c_str());
        ImGui::Text("Entity: %s", entityID.c_str());
        ImGui::Separator();

        ImGui::Text("Position");
        ImGui::InputFloat3("Position", glm::value_ptr(transform.position));
        // ImGui::InputFloat3("", glm::value_ptr(transform.position));

        ImGui::Text("Rotation (Euler Angles)");
        glm::vec3 eulerAngles = QuaternionToEuler(transform.rotation);
        if (ImGui::InputFloat3("Rotation", glm::value_ptr(eulerAngles))) {
            transform.rotation = EulerToQuaternion(eulerAngles);
        }

        ImGui::Text("Scale");
        ImGui::InputFloat3("Scale", glm::value_ptr(transform.scale));

        // Update the entity's transform component with the modified values only if they have changed
        if (transform.position != entityPosition) {
            entityTransformComponent->SetPosition(transform.position);
        }
        if (transform.rotation != entityRotation) {
            entityTransformComponent->SetRotation(transform.rotation);
        }
        if (transform.scale != entityScale) {
            entityTransformComponent->SetScale(transform.scale);
        }

        if (ImGui::SmallButton("delete")) {
            m_EntityManager->RemoveEntity(entity->GetID());
        }

        ImGui::End();

        index++;
    }

    // static bool show = true;
    // ImGui::ShowDemoWindow(&show);

    // Rendering

    ImGui::Render();
    // ENGINE_WARN("Rendering data");
    ImDrawData *draw_data = ImGui::GetDrawData();
    FrameRender(draw_data);
}
void ImGuiLayer::FrameRender(ImDrawData *draw_data) {
    Engine::Application &app = Engine::Application::Get();
    auto renderer = static_cast<Engine::VulkanRenderer_refac *>(app.GetRenderer());
    auto window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());
    VkResult err;

    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = renderer->GetRenderPass()->getRenderPass();
        // info.framebuffer = vulkandata.swapChainFramebuffers[imageIndex];
        info.framebuffer = renderer->GetFrameBuffer()->getFrameBuffers()[renderer->GetCurrentFrame()];

        info.renderArea.extent.width = app.GetWindow().GetWidth();
        info.renderArea.extent.height = app.GetWindow().GetHeight();
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        info.pClearValues = clearValues.data();

        ImGui_ImplVulkan_Data *gui_vulkandata = ImGui_ImplVulkan_GetBackendData();
        // vkCmdBindPipeline(vulkandata.commandBuffers[vulkandata.currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
        //                   gui_vulkandata->Pipeline);

        vkCmdBindPipeline(renderer->GetCommandBuffer()->getCommandBuffers()[renderer->GetCurrentFrame()],
                          VK_PIPELINE_BIND_POINT_GRAPHICS, gui_vulkandata->Pipeline);

        // vkCmdBeginRenderPass(vulkandata.commandBuffers[vulkandata.currentFrame],
        //     &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data,
                                    renderer->GetCommandBuffer()->getCommandBuffers()[renderer->GetCurrentFrame()]);

    // Submit command buffer
    // vkCmdEndRenderPass(renderer->GetCommandBuffer()->getCommandBuffers()[renderer->GetCurrentFrame()]);
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
void ImGuiLayer::OnEvent(Engine::Event &event) {
    // Engine::EventDispatcher dispatcher(event);
    // dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
    //     BIND_EVENT_FN(OnMouseButtonPressedEvent));
    // dispatcher.Dispatch<Engine::MouseButtonReleasedEvent>(
    //     BIND_EVENT_FN(OnMouseButtonReleasedEvent));
    // dispatcher.Dispatch<Engine::MouseMovedEvent>(
    //     BIND_EVENT_FN(OnMouseMovedEvent));
    // dispatcher.Dispatch<Engine::MouseScrolledEvent>(
    //     BIND_EVENT_FN(OnMouseScrolledEvent));
    // dispatcher.Dispatch<Engine::KeyPressedEvent>(
    //     BIND_EVENT_FN(OnKeyPressedEvent));
    // dispatcher.Dispatch<Engine::KeyReleasedEvent>(
    //     BIND_EVENT_FN(OnKeyReleasedEvent));
    // dispatcher.Dispatch<Engine::WindowResizeEvent>(
    //     BIND_EVENT_FN(OnWindowResizeEvent));
    // dispatcher.Dispatch<Engine::KeyTypedEvent>(BIND_EVENT_FN(OnKeyTypedEvent));
}

bool ImGuiLayer::OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(e.GetMouseButton(), true);

    return false;
}
bool ImGuiLayer::OnMouseButtonReleasedEvent(Engine::MouseButtonReleasedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseButtonEvent(e.GetMouseButton(), false);

    return false;
}
bool ImGuiLayer::OnMouseMovedEvent(Engine::MouseMovedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(e.GetX(), e.GetY());

    return false;
}
bool ImGuiLayer::OnMouseScrolledEvent(Engine::MouseScrolledEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());

    return false;
}
bool ImGuiLayer::OnKeyPressedEvent(Engine::KeyPressedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    int mods = e.GetMods();
    if (int keycode_to_mod = ImGui_ImplGlfw_KeyToModifier(e.GetKeyCode()))
        mods = (mods | keycode_to_mod);

    ImGui_ImplGlfw_UpdateKeyModifiers(mods);
    ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
    io.AddKeyEvent(imgui_key, true);

    return false;
}
bool ImGuiLayer::OnKeyReleasedEvent(Engine::KeyReleasedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
    io.AddKeyEvent(imgui_key, false);

    return false;
}
bool ImGuiLayer::OnWindowResizeEvent(Engine::WindowResizeEvent &e) { return false; }

bool ImGuiLayer::OnKeyTypedEvent(Engine::KeyTypedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(e.GetCharacter());
    return false;
}

} // namespace Editor
