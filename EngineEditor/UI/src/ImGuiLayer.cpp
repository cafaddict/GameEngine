#include "ImGuiLayer.hpp"

namespace Editor {
#define BIND_EVENT_FN(x) std::bind(&ImGuiLayer::x, this, std::placeholders::_1)
ImGuiLayer::ImGuiLayer() : Engine::Layer("ImGui Layer") {}
ImGuiLayer::~ImGuiLayer() {}
void ImGuiLayer::OnAttach() {
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
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000;
  pool_info.poolSizeCount = std::size(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;

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
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

  ImGui_ImplVulkan_Init(&init_info, vulkandata.renderPass);

  VkCommandBuffer cmd = vulkandata.commandBuffers[vulkandata.currentFrame];
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;                   // Optional
  beginInfo.pInheritanceInfo = nullptr;  // Optional

  vkBeginCommandBuffer(cmd, &beginInfo);

  ImGui_ImplVulkan_CreateFontsTexture(cmd);

  vkEndCommandBuffer(cmd);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {
      vulkandata.imageAvailableSemaphores[vulkandata.currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers =
      &vulkandata.commandBuffers[vulkandata.currentFrame];

  VkSemaphore signalSemaphores[] = {
      vulkandata.renderFinishedSemaphores[vulkandata.currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkQueueSubmit(vulkandata.graphicsQueue, 1, &submitInfo,
                vulkandata.inFlightFences[vulkandata.currentFrame]);

  vkWaitForFences(vulkandata.device, 1,
                  &vulkandata.inFlightFences[vulkandata.currentFrame], true,
                  9999999999);
  vkResetFences(vulkandata.device, 1,
                &vulkandata.inFlightFences[vulkandata.currentFrame]);

  // reset the command buffers inside the command pool
  vkResetCommandPool(vulkandata.device, vulkandata.commandPool, 0);

  // clear font textures from cpu data
  ImGui_ImplVulkan_DestroyFontUploadObjects();
}
void ImGuiLayer::OnDetach() {
  Engine::Application& app = Engine::Application::Get();
  auto renderer = static_cast<Engine::VulkanRenderer*>(app.GetRenderer());
  auto vulkandata = renderer->GetVulkanData();
  vkDestroyDescriptorPool(vulkandata.device, imguiPool, nullptr);
  ImGui_ImplVulkan_Shutdown();
}
void ImGuiLayer::OnUpdate() {
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

  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  static bool show = true;
  ImGui::ShowDemoWindow(&show);
  // Rendering
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(
      ImGui::GetDrawData(), vulkandata.commandBuffers[vulkandata.currentFrame]);
}
void ImGuiLayer::OnWait() {}
void ImGuiLayer::OnEvent(Engine::Event& event) {
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

bool ImGuiLayer::OnMouseButtonPressedEvent(Engine::MouseButtonPressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseButtonEvent(e.GetMouseButton(), true);

  return false;
}
bool ImGuiLayer::OnMouseButtonReleasedEvent(
    Engine::MouseButtonReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseButtonEvent(e.GetMouseButton(), false);

  return false;
}
bool ImGuiLayer::OnMouseMovedEvent(Engine::MouseMovedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(e.GetX(), e.GetY());

  return false;
}
bool ImGuiLayer::OnMouseScrolledEvent(Engine::MouseScrolledEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());

  return false;
}
bool ImGuiLayer::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  int mods = e.GetMods();
  if (int keycode_to_mod = ImGui_ImplGlfw_KeyToModifier(e.GetKeyCode()))
    mods = (mods | keycode_to_mod);

  ImGui_ImplGlfw_UpdateKeyModifiers(mods);
  ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
  io.AddKeyEvent(imgui_key, true);

  return false;
}
bool ImGuiLayer::OnKeyReleasedEvent(Engine::KeyReleasedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(e.GetKeyCode());
  io.AddKeyEvent(imgui_key, false);

  return false;
}
bool ImGuiLayer::OnWindowResizeEvent(Engine::WindowResizeEvent& e) {
  return false;
}

bool ImGuiLayer::OnKeyTypedEvent(Engine::KeyTypedEvent& e) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddInputCharacter(e.GetCharacter());
  return false;
}

}  // namespace Editor