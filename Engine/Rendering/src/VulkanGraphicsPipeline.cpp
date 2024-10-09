#include "VulkanGraphicsPipeline.hpp"
#include "Log.hpp"
#include "vulkan/vulkan_core.h"
#include <array>
#define MAX_FRAMES_IN_FLIGHT 2
namespace Engine {

VulkanGraphicsPipeline::VulkanGraphicsPipeline(std::shared_ptr<VulkanDevice> device,
                                               std::shared_ptr<VulkanRenderPass> renderPass, VulkanShadersData shaders,
                                               VulkanBaseVertex &inputStruct)
    : m_Device(device), m_RenderPass(renderPass), m_Shaders(shaders) {
    if (shaders.computeShader.has_value()) {
        createComputeDescriptorSetLayout();
        createPipelineLayout(m_ComputeDescriptorsetLayout);
        createComputePipeline(shaders);
    }
    createDescriptorSetLayout();
    createPipelineLayout(m_DescriptorsetLayout);
    createGraphicsPipeline(shaders, inputStruct);
}
VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    ENGINE_INFO("VulaknGraphicsPipeline is destroyed");
    vkDestroyPipeline(m_Device->getLogicalDevice(), m_GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_Device->getLogicalDevice(), m_PipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(m_Device->getLogicalDevice(), m_DescriptorsetLayout, nullptr);
}

void VulkanGraphicsPipeline::createGraphicsPipeline(VulkanShadersData shaders, VulkanBaseVertex &inputStruct) {
    auto vertShaderCode = shaders.vertexShader.value();
    auto fragShaderCode = shaders.fragShader.value();
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    auto bindingDescription = inputStruct.getBindingDescription();
    auto attributeDescriptions = inputStruct.getAttributeDescriptions();
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());

    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_TRUE; // enable sample shading in the pipeline
    multisampling.minSampleShading = .2f;        // min fraction for sample shading; closer to one is smoother
    multisampling.rasterizationSamples = m_Device->getMsaaSamples();

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {};  // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_RenderPass->getRenderPass();
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(m_Device->getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                  &m_GraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(m_Device->getLogicalDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(m_Device->getLogicalDevice(), vertShaderModule, nullptr);
}

void VulkanGraphicsPipeline::createComputePipeline(VulkanShadersData shaders) {
    auto computeShaderCode = shaders.computeShader.value();
    VkShaderModule computeShaderModule = createShaderModule(computeShaderCode);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

    VkComputePipelineCreateInfo computePipelineInfo{};
    computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineInfo.stage = computeShaderStageInfo;
    computePipelineInfo.layout = m_ComputePipelineLayout;

    if (vkCreateComputePipelines(m_Device->getLogicalDevice(), VK_NULL_HANDLE, 1, &computePipelineInfo, nullptr,
                                 &m_ComputePipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }
}

VkShaderModule VulkanGraphicsPipeline::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_Device->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}
void VulkanGraphicsPipeline::createPipelineLayout(VkDescriptorSetLayout &descriptorSetLayout) {
    createDescriptorSetLayout();
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(m_Device->getLogicalDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void VulkanGraphicsPipeline::createDescriptorSetLayout() {
    // Binding 0: Uniform buffer: Camera
    VkDescriptorSetLayoutBinding cameraUboLayoutBinding{};
    cameraUboLayoutBinding.binding = 0;
    cameraUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraUboLayoutBinding.descriptorCount = 1;
    cameraUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    cameraUboLayoutBinding.pImmutableSamplers = nullptr;

    // Binding 1: Uniform buffer: Light
    VkDescriptorSetLayoutBinding lightUboLayoutBinding{};
    lightUboLayoutBinding.binding = 1;
    lightUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lightUboLayoutBinding.descriptorCount = 1;
    lightUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    lightUboLayoutBinding.pImmutableSamplers = nullptr;

    // Binding 2: Texture sampler
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 2;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    // Binding 3: SSBO for transformations
    VkDescriptorSetLayoutBinding storageBufferBinding{};
    storageBufferBinding.binding = 3;
    storageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferBinding.descriptorCount = 1;
    storageBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    storageBufferBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 4> bindings = {cameraUboLayoutBinding, lightUboLayoutBinding,
                                                            samplerLayoutBinding, storageBufferBinding};
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    if (vkCreateDescriptorSetLayout(m_Device->getLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorsetLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void VulkanGraphicsPipeline::createComputeDescriptorSetLayout() {

    // Binding 0: Uniform buffer
    VkDescriptorSetLayoutBinding uniformBufferBinding = {};
    uniformBufferBinding.binding = 0;
    uniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferBinding.descriptorCount = 1;
    uniformBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    uniformBufferBinding.pImmutableSamplers = nullptr;

    // Binding 1: Storage buffer (input)
    VkDescriptorSetLayoutBinding storageBufferBinding{};
    storageBufferBinding.binding = 1;
    storageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferBinding.descriptorCount = 1;
    storageBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    storageBufferBinding.pImmutableSamplers = nullptr;

    // Binding 2: Storage buffer (output)
    VkDescriptorSetLayoutBinding storageBufferDynamicBinding{};
    storageBufferDynamicBinding.binding = 2;
    storageBufferDynamicBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    storageBufferDynamicBinding.descriptorCount = 1;
    storageBufferDynamicBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    storageBufferDynamicBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = {uniformBufferBinding, storageBufferBinding,
                                                            storageBufferDynamicBinding};

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutCreateInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_Device->getLogicalDevice(), &layoutCreateInfo, nullptr,
                                    &m_ComputeDescriptorsetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }
}
} // namespace Engine