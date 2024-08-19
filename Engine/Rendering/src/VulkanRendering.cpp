#include <VulkanRendering.hpp>
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#include <stb_image/stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>
#include <random>

namespace Engine
{
    // This is only for testing remove it
    // const std::vector<Vertex> vertices = {
    //     {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    //     {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    //     {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};
    // debugCallback
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void *pUserData)
    {
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            ENGINE_TRACE("validation layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            ENGINE_INFO("validation layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            ENGINE_WARN("validation layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            ENGINE_ERROR("validation layer: {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            ENGINE_WARN("validation layer: {0}", pCallbackData->pMessage);
            break;
        }
        return VK_FALSE;
    }

    // With window
    Renderer *Renderer::Create(GLFWwindow *window)
    {
        ENGINE_WARN("Vulkan Renderer Creation");
        return new VulkanRenderer(window);
    }

    VulkanRenderer::VulkanRenderer(GLFWwindow *window)
    {
        SetWindow(window);
        Init();
    }
    // Without window
    Renderer *Renderer::Create() { return new VulkanRenderer(); }
    VulkanRenderer::VulkanRenderer() { Init(); }

    VulkanRenderer::~VulkanRenderer() { Shutdown(); }

    // Later support offscreen rendering
    void VulkanRenderer::Init()
    {
        createInstance();
        setupDebugMessenger();
        if (m_VulkanData.window != nullptr)
            createSurface(); // For now alway need window
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGUIRenderPass();

        createDescriptorSetLayout();
        createComputeDescriptorSetLayout();

        // createGraphicsPipeline();
        createComputePipeline();

        createCommandPool();
        createComputeCommandPool();

        createColorResources();
        createDepthResources();
        createFramebuffer();
        createGUIFramebuffer();

        // createTextureImage();
        // createTextureImageView();
        // createTextureSampler();

        createUniformBuffers();

        VkDeviceSize buffersize = sizeof(glm::mat4) * 1000;
        createShaderStorageBuffers(buffersize);

        // createDescriptorPool();
        // createDescriptorSets();

        createCommandBuffer();
        createComputeCommandBuffer();

        createSyncObjects();

        // loadModel();
        // createVertexBuffer();
        // createIndexBuffer();
    }

    void VulkanRenderer::recreateSwapChain()
    {
        while (m_VulkanData.minimized)
        {
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_VulkanData.device);

        cleanupSwapChain();
        createSwapChain();
        createImageViews();
        createColorResources();
        createDepthResources();
        createFramebuffer();
        createGUIFramebuffer();
    }

    void VulkanRenderer::BeginRecord()
    {
        vkWaitForFences(m_VulkanData.device, 1,
                        &m_VulkanData.inFlightFences[m_VulkanData.currentFrame],
                        VK_TRUE, UINT64_MAX);

        // uint32_t imageIndex;

        VkResult result = vkAcquireNextImageKHR(
            m_VulkanData.device, m_VulkanData.swapChain, UINT64_MAX,
            m_VulkanData.imageAvailableSemaphores[m_VulkanData.currentFrame],
            VK_NULL_HANDLE, &m_VulkanData.image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        updateUniformBuffer(m_VulkanData.currentFrame);

        std::vector<glm::mat4> transformations_in;
        auto Entities = m_EntityManager->GetAllEntities();

        for (auto entity : Entities)
        {

            auto transform = entity->GetComponent<TransformComponent>()->GetTransformMatrix();
            transformations_in.push_back(transform);
        }

        dynamic_cast<VulkanShaderStorageBuffer<glm::mat4> *>(m_ShaderStorageBuffers.get())->Update(m_VulkanData.currentFrame, transformations_in);

        // Only reset the fence if we are submitting work
        vkResetFences(m_VulkanData.device, 1,
                      &m_VulkanData.inFlightFences[m_VulkanData.currentFrame]);

        vkResetCommandBuffer(m_VulkanData.commandBuffers[m_VulkanData.currentFrame],
                             /*VkCommandBufferResetFlagBits*/ 1);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(m_VulkanData.commandBuffers[m_VulkanData.currentFrame], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        recordCommandBuffer(m_VulkanData.commandBuffers[m_VulkanData.currentFrame],
                            m_VulkanData.image_index);

        // VkSubmitInfo submitInfo{};
        // submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        // VkSemaphore waitSemaphores[] = {
        //     m_VulkanData.imageAvailableSemaphores[m_VulkanData.currentFrame] };
        // VkPipelineStageFlags waitStages[] = {
        //     VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        // submitInfo.waitSemaphoreCount = 1;
        // submitInfo.pWaitSemaphores = waitSemaphores;
        // submitInfo.pWaitDstStageMask = waitStages;

        // submitInfo.commandBufferCount = 1;
        // submitInfo.pCommandBuffers =
        //     &m_VulkanData.commandBuffers[m_VulkanData.currentFrame];

        // VkSemaphore signalSemaphores[] = {
        //     m_VulkanData.renderFinishedSemaphores[m_VulkanData.currentFrame] };
        // submitInfo.signalSemaphoreCount = 1;
        // submitInfo.pSignalSemaphores = signalSemaphores;

        // if (vkQueueSubmit(m_VulkanData.graphicsQueue, 1, &submitInfo,
        //     m_VulkanData.inFlightFences[m_VulkanData.currentFrame]) !=
        //     VK_SUCCESS)
        //     {
        //     throw std::runtime_error("failed to submit draw command buffer!");
        //     }
    }

    void VulkanRenderer::createEntityResources()
    {
        ENGINE_TRACE("create entity resource start");
        size_t currentVertexOffset = 0;
        size_t currentIndexOffset = 0;
        auto entities = m_EntityManager->GetAllEntities();
        vertices.clear();
        indices.clear();

        // entityBufferInfos.clear();
        // entityPipelines.clear();
        // entityTextures.clear();
        // entityDescriptorSets.clear();

        int cnt = 0;
        for (const auto &entity : entities)
        {
            auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();
            auto texture_data = entity->GetComponent<TextureComponent>()->GetTextureData();
            auto vertex_shader_data = entity->GetComponent<ShaderComponent>()->GetVertexShader();
            auto fragment_shader_data = entity->GetComponent<ShaderComponent>()->GetFragmentShader();
            auto compute_shader_data = entity->GetComponent<ShaderComponent>()->GetComputeShader();

            ENGINE_TRACE("modelData size : {0}", model_data->positions.size());

            // size_t vertexCount = model_data->positions.size();
            // size_t indexCount = model_data->indices.size();
            size_t vertexCount = model_data->positions.size();
            size_t indexCount = model_data->indices.size();

            EntityBufferInfo bufferInfo;
            bufferInfo.vertexOffset = currentVertexOffset;
            bufferInfo.indexOffset = currentIndexOffset;
            bufferInfo.vertexCount = vertexCount;
            bufferInfo.indexCount = indexCount;

            entityBufferInfos[entity] = bufferInfo;

            for (size_t i = 0; i < model_data->indices.size(); i++)
            {

                Vertex vertex{};
                vertex.pos = model_data->positions[i];
                vertex.normal = model_data->normals.size() > i ? model_data->normals[i] : glm::vec3(0.0f, 0.0f, 1.0f);
                vertex.texCoord = model_data->normals.size() > i ? model_data->uvs[i] : glm::vec2(0.0f);
                vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
                vertices.push_back(vertex);
            }

            indices.insert(indices.end(), model_data->indices.begin(), model_data->indices.end());
            auto entityTransformation = entity->GetComponent<TransformComponent>();
            glm::mat4 transformMatrix = entityTransformation->GetTransformMatrix();
            transformations.push_back(transformMatrix);

            currentVertexOffset += vertexCount;
            currentIndexOffset += indexCount;

            VkShaderModule vertShaderModule = createShaderModule(vertex_shader_data->GetShaderCode());
            VkShaderModule fragShaderModule = createShaderModule(fragment_shader_data->GetShaderCode());

            PipelineData pipelineData = createGraphicsPipeline(vertShaderModule, fragShaderModule);
            entityPipelines[entity] = pipelineData;

            vkDestroyShaderModule(m_VulkanData.device, fragShaderModule, nullptr);
            vkDestroyShaderModule(m_VulkanData.device, vertShaderModule, nullptr);

            int texWidth = texture_data->width;
            int texHeight = texture_data->height;
            VulkanTextureData vulkanTextureData;
            vulkanTextureData.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
            VkDeviceSize imageSize = texture_data->width * texture_data->height * 4;

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            VulkanBuffer vulkanbuffer(&m_VulkanData);
            vulkanbuffer.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      stagingBuffer, stagingBufferMemory);
            void *data;
            vkMapMemory(m_VulkanData.device, stagingBufferMemory, 0, imageSize, 0, &data);
            memcpy(data, texture_data->pixels.get(), static_cast<size_t>(imageSize));
            vkUnmapMemory(m_VulkanData.device, stagingBufferMemory);

            createImage(texture_data->width, texture_data->height, vulkanTextureData.mipLevels,
                        VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vulkanTextureData.textureImage,
                        vulkanTextureData.textureImageMemory);

            transitionImageLayout(vulkanTextureData.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  vulkanTextureData.mipLevels);
            copyBufferToImage(stagingBuffer, vulkanTextureData.textureImage,
                              static_cast<uint32_t>(texWidth),
                              static_cast<uint32_t>(texHeight));

            vkDestroyBuffer(m_VulkanData.device, stagingBuffer, nullptr);
            vkFreeMemory(m_VulkanData.device, stagingBufferMemory, nullptr);

            generateMipmaps(vulkanTextureData.textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth,
                            texHeight, vulkanTextureData.mipLevels);

            vulkanTextureData.textureImageView = createImageView(vulkanTextureData.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                                                                 VK_IMAGE_ASPECT_COLOR_BIT, vulkanTextureData.mipLevels);

            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;

            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

            samplerInfo.anisotropyEnable = VK_TRUE;

            VkPhysicalDeviceProperties properties{};
            vkGetPhysicalDeviceProperties(m_VulkanData.physicalDevice, &properties);
            samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = static_cast<float>(vulkanTextureData.mipLevels);

            if (vkCreateSampler(m_VulkanData.device, &samplerInfo, nullptr,
                                &vulkanTextureData.textureImageSampler) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create texture sampler!");
            }

            entityTextures[entity] = vulkanTextureData;
            createDescriptorPool();
            createDescriptorSets(entity, vulkanTextureData);
            cnt++;
        }

        createVertexBuffer();
        createIndexBuffer();

        ENGINE_TRACE("create entity resource finished");
    }

    VulkanRenderer::PipelineData VulkanRenderer::createGraphicsPipeline(VkShaderModule vertshaderModule, VkShaderModule fragshaderModule)
    {
        VkPipeline graphicsPipeline;
        VkPipelineLayout pipelineLayout;

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

        vertShaderStageInfo.module = vertshaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragshaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                          fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());

        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
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
        multisampling.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable =
            VK_TRUE; // enable sample shading in the pipeline
        multisampling.minSampleShading =
            .2f; // min fraction for sample shading; closer to one is smoother
        multisampling.rasterizationSamples = m_VulkanData.msaaSamples;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
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
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_VulkanData.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(m_VulkanData.device, &pipelineLayoutInfo, nullptr,
                                   &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

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

        pipelineInfo.layout = pipelineLayout;

        pipelineInfo.renderPass = m_VulkanData.renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1;              // Optional

        if (vkCreateGraphicsPipelines(m_VulkanData.device, VK_NULL_HANDLE, 1,
                                      &pipelineInfo, nullptr,
                                      &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        PipelineData pipelineData;
        pipelineData.graphicsPipeline = graphicsPipeline;
        pipelineData.pipelineLayout = pipelineLayout;
        return pipelineData;
    }

    void VulkanRenderer::addModel(std::string model_path)
    {

        auto entities = m_EntityManager->GetAllEntities();

        for (const auto &entity : entities)
        {
            auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();
            for (int i = 0; i < model_data->indices.size(); i++)
            {

                Vertex vertex{};
                vertex.pos = model_data->positions[i];
                vertex.texCoord = model_data->uvs[i];
                vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
                vertices.push_back(vertex);
            }
            // indices = model_data->indices;
            indices.insert(indices.end(), model_data->indices.begin(), model_data->indices.end());
        }

        // ModelData* model_data = new ModelData();
        // model_data->Load(model_path);
        // for (int i = 0; i < model_data->indices.size(); i++) {
        //     Vertex vertex{};
        //     vertex.pos = model_data->positions[i];
        //     vertex.texCoord = model_data->uvs[i];
        //     vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        //     vertices.push_back(vertex);
        //     }
        // indices = model_data->indices;

        // loadModel();

        // tinyobj::attrib_t attrib;
        // std::vector<tinyobj::shape_t> shapes;
        // std::vector<tinyobj::material_t> materials;
        // std::string warn, err;

        // if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
        //     model_path.c_str()))
        //     {
        //     throw std::runtime_error(warn + err);
        //     }

        // std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        // for (const auto& shape : shapes)
        //     {
        //     for (const auto& index : shape.mesh.indices)
        //         {
        //         Vertex vertex{};

        //         vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
        //                       attrib.vertices[3 * index.vertex_index + 1],
        //                       attrib.vertices[3 * index.vertex_index + 2] };

        //         vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
        //                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

        //         vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

        //         if (uniqueVertices.count(vertex) == 0)
        //             {
        //             uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        //             vertices.push_back(vertex);
        //             }

        //         indices.push_back(uniqueVertices[vertex]);
        //         }
        //     }

        createVertexBuffer();
        createIndexBuffer();
    }

    void VulkanRenderer::Draw()
    {

        if (vkEndCommandBuffer(m_VulkanData.commandBuffers[m_VulkanData.currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {
            m_VulkanData.imageAvailableSemaphores[m_VulkanData.currentFrame]};
        VkPipelineStageFlags waitStages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers =
            &m_VulkanData.commandBuffers[m_VulkanData.currentFrame];

        VkSemaphore signalSemaphores[] = {
            m_VulkanData.renderFinishedSemaphores[m_VulkanData.currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_VulkanData.graphicsQueue, 1, &submitInfo,
                          m_VulkanData.inFlightFences[m_VulkanData.currentFrame]) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        //     VkSemaphore signalSemaphores[] = {
        // m_VulkanData.renderFinishedSemaphores[m_VulkanData.currentFrame] };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_VulkanData.swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_VulkanData.image_index;
        presentInfo.pResults = nullptr; // Optional

        VkResult result = vkQueuePresentKHR(m_VulkanData.presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            m_VulkanData.framebufferResized)
        {
            m_VulkanData.framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_VulkanData.currentFrame =
            (m_VulkanData.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VulkanRenderer::Shutdown()
    {
        cleanupSwapChain();

        vkDestroyPipeline(m_VulkanData.device, m_VulkanData.graphicsPipeline,
                          nullptr);
        vkDestroyPipelineLayout(m_VulkanData.device, m_VulkanData.pipelineLayout,
                                nullptr);
        vkDestroyRenderPass(m_VulkanData.device, m_VulkanData.renderPass, nullptr);
        vkDestroyRenderPass(m_VulkanData.device, m_VulkanData.GUIrenderPass, nullptr);

        m_UniformBuffers->Destroy();

        vkDestroyDescriptorPool(m_VulkanData.device, m_VulkanData.descriptorPool,
                                nullptr);

        vkDestroySampler(m_VulkanData.device, m_VulkanData.textureSampler, nullptr);
        vkDestroyImageView(m_VulkanData.device, m_VulkanData.textureImageView,
                           nullptr);

        vkDestroyImage(m_VulkanData.device, m_VulkanData.textureImage, nullptr);
        vkFreeMemory(m_VulkanData.device, m_VulkanData.textureImageMemory, nullptr);

        vkDestroyDescriptorSetLayout(m_VulkanData.device,
                                     m_VulkanData.descriptorSetLayout, nullptr);

        m_IndexBuffer->Destroy();
        m_VertexBuffer->Destroy();
        m_UniformBuffers->Destroy();
        // dynamic_cast<VulkanShaderStorageBuffer<Particle>*>(m_ShaderStorageBuffers.get())->Destroy();
        // m_ShaderStorageBuffers->Destroy();

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(m_VulkanData.device,
                               m_VulkanData.renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_VulkanData.device,
                               m_VulkanData.imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_VulkanData.device,
                               m_VulkanData.computeFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_VulkanData.device, m_VulkanData.inFlightFences[i],
                           nullptr);
        }

        vkDestroyCommandPool(m_VulkanData.device, m_VulkanData.commandPool, nullptr);
        vkDestroyCommandPool(m_VulkanData.device, m_VulkanData.computeCommandPool, nullptr);

        vkDestroyDevice(m_VulkanData.device, nullptr);

        if (enableValidationLayers)
        {
            auto DestroyDebugUtilsMessengerEXT =
                [](VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                   const VkAllocationCallbacks *pAllocator)
            {
                auto func =
                    (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                        instance, "vkDestroyDebugUtilsMessengerEXT");
                if (func != nullptr)
                {
                    func(instance, debugMessenger, pAllocator);
                }
            };
            DestroyDebugUtilsMessengerEXT(m_VulkanData.instance,
                                          m_VulkanData.debugMessenger, nullptr);
        }
        vkDestroySurfaceKHR(m_VulkanData.instance, m_VulkanData.surface, nullptr);
        vkDestroyInstance(m_VulkanData.instance, nullptr);
    }

    /*
    =============================================
    Init Functions
    =============================================
    */

    void VulkanRenderer::createInstance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Renderer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "CafAddict Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto glfwExtensions = getRequiredExtensions();
        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(glfwExtensions.size());
        createInfo.ppEnabledExtensionNames = glfwExtensions.data();

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               extensions.data());
        bool portabilitySubsetSupported = false;
        for (const auto &extension : extensions)
        {
            std::cout << '\t' << extension.extensionName << std::endl;
            if (strcmp(extension.extensionName, "VK_KHR_portability_subset") == 0)
            {
                portabilitySubsetSupported = true;
                break;
            }
        }

        if (portabilitySubsetSupported)
        {
            createInfo.enabledExtensionCount = 1;
            const char *extensionName = "VK_KHR_portability_subset";
            createInfo.ppEnabledExtensionNames = &extensionName;
        }
        else
        {
            std::cout << "there is no portabilitySubsetSupported" << std::endl;
        }

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount =
                static_cast<uint32_t>(m_VulkanData.validationLayers.size());
            createInfo.ppEnabledLayerNames = m_VulkanData.validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &m_VulkanData.instance))
        {
            throw std::runtime_error("failed to create instance!");
        }

        // std::cout << "available extensions:\n";

        // for (const auto& extension : extensions) {
        //   std::cout << '\t' << extension.extensionName << '\n';
        //   }
    }

    bool VulkanRenderer::checkValidationLayerSupport()
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : m_VulkanData.validationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void VulkanRenderer::populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void VulkanRenderer::setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        auto CreateDebugUtilsMessengerEXT =
            [](VkInstance instance,
               const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
               const VkAllocationCallbacks *pAllocator,
               VkDebugUtilsMessengerEXT *pDebugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            }
            else
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        };
        if (CreateDebugUtilsMessengerEXT(m_VulkanData.instance, &createInfo, nullptr,
                                         &m_VulkanData.debugMessenger) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void VulkanRenderer::pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VulkanData.instance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            throw std::runtime_error("failed  to find GPUs with Vulkan support!");
        }
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_VulkanData.instance, &deviceCount,
                                   devices.data());

        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device))
            {
                m_VulkanData.physicalDevice = device;
                m_VulkanData.msaaSamples = getMaxUsableSampleCount();
                break;
            }
        }

        if (m_VulkanData.physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void VulkanRenderer::createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(m_VulkanData.physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                                  indices.presentFamily.value(),
                                                  indices.computeFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        deviceFeatures.sampleRateShading =
            VK_TRUE; // enable sample shading feature for the device

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount =
            static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(m_VulkanData.deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_VulkanData.deviceExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount =
                static_cast<uint32_t>(m_VulkanData.validationLayers.size());
            createInfo.ppEnabledLayerNames = m_VulkanData.validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_VulkanData.physicalDevice, &createInfo, nullptr,
                           &(m_VulkanData.device)) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_VulkanData.device, indices.graphicsFamily.value(), 0,
                         &(m_VulkanData.graphicsQueue));
        vkGetDeviceQueue(m_VulkanData.device, indices.computeFamily.value(), 0,
                         &(m_VulkanData.computeQueue));
        vkGetDeviceQueue(m_VulkanData.device, indices.presentFamily.value(), 0,
                         &m_VulkanData.presentQueue);
    }

    void VulkanRenderer::createSurface()
    {
        if (glfwCreateWindowSurface(m_VulkanData.instance, m_VulkanData.window,
                                    nullptr, &m_VulkanData.surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VulkanRenderer::createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport =
            querySwapChainSupport(m_VulkanData.physicalDevice);

        VkSurfaceFormatKHR surfaceFormat =
            chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode =
            chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_VulkanData.surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(m_VulkanData.physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                         indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;     // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_VulkanData.device, &createInfo, nullptr,
                                 &m_VulkanData.swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_VulkanData.device, m_VulkanData.swapChain,
                                &imageCount, nullptr);
        m_VulkanData.swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_VulkanData.device, m_VulkanData.swapChain,
                                &imageCount, m_VulkanData.swapChainImages.data());

        m_VulkanData.swapChainImageFormat = surfaceFormat.format;
        m_VulkanData.swapChainExtent = extent;
    }

    void VulkanRenderer::createImageViews()
    {
        m_VulkanData.swapChainImageViews.resize(m_VulkanData.swapChainImages.size());
        for (uint32_t i = 0; i < m_VulkanData.swapChainImages.size(); i++)
        {
            m_VulkanData.swapChainImageViews[i] = createImageView(
                m_VulkanData.swapChainImages[i], m_VulkanData.swapChainImageFormat,
                VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }
    }

    VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format,
                                                VkImageAspectFlags aspectFlags,
                                                uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(m_VulkanData.device, &viewInfo, nullptr, &imageView) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void VulkanRenderer::createRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_VulkanData.swapChainImageFormat;
        colorAttachment.samples = m_VulkanData.msaaSamples;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = m_VulkanData.msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = m_VulkanData.swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments = {
            colorAttachment, depthAttachment, colorAttachmentResolve};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_VulkanData.device, &renderPassInfo, nullptr,
                               &m_VulkanData.renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanRenderer::createGUIRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_VulkanData.swapChainImageFormat;
        colorAttachment.samples = m_VulkanData.msaaSamples;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = m_VulkanData.msaaSamples;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachmentResolve{};
        colorAttachmentResolve.format = m_VulkanData.swapChainImageFormat;
        colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentResolveRef{};
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colorAttachmentResolveRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 3> attachments = {
            colorAttachment, depthAttachment, colorAttachmentResolve};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_VulkanData.device, &renderPassInfo, nullptr,
                               &m_VulkanData.GUIrenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanRenderer::createDescriptorSetLayout()
    {
        // // Binding 0: Uniform buffer (e.g., camera and lighting UBO)
        // VkDescriptorSetLayoutBinding uboLayoutBinding{};
        // uboLayoutBinding.binding = 0;
        // uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // uboLayoutBinding.descriptorCount = 1;
        // uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        // uboLayoutBinding.pImmutableSamplers = nullptr;

        // // Binding 1: Texture sampler
        // VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        // samplerLayoutBinding.binding = 1;
        // samplerLayoutBinding.descriptorCount = 1;
        // samplerLayoutBinding.descriptorType =
        //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // samplerLayoutBinding.pImmutableSamplers = nullptr;
        // samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        // std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding,
        //                                                         samplerLayoutBinding };
        // VkDescriptorSetLayoutCreateInfo layoutInfo{};
        // layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        // layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        // layoutInfo.pBindings = bindings.data();
        // if (vkCreateDescriptorSetLayout(m_VulkanData.device, &layoutInfo, nullptr,
        //     &m_VulkanData.descriptorSetLayout) !=
        //     VK_SUCCESS)
        //     {
        //     throw std::runtime_error("failed to create descriptor set layout!");
        //     }

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
        samplerLayoutBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
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
        if (vkCreateDescriptorSetLayout(m_VulkanData.device, &layoutInfo, nullptr,
                                        &m_VulkanData.descriptorSetLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void VulkanRenderer::createComputeDescriptorSetLayout()
    {

        VkDescriptorSetLayoutBinding uniformBufferBinding = {};
        uniformBufferBinding.binding = 0;
        uniformBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBufferBinding.descriptorCount = 1;
        uniformBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; // Use in compute shader
        uniformBufferBinding.pImmutableSamplers = nullptr;             // Not used for buffers

        VkDescriptorSetLayoutBinding storageBufferBinding{};
        storageBufferBinding.binding = 1;
        storageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        storageBufferBinding.descriptorCount = 1;
        storageBufferBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        storageBufferBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding storageBufferDynamicBinding{};
        storageBufferDynamicBinding.binding = 2;
        storageBufferDynamicBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        storageBufferDynamicBinding.descriptorCount = 1;
        storageBufferDynamicBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        storageBufferDynamicBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings = {uniformBufferBinding, storageBufferBinding, storageBufferDynamicBinding};

        VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutCreateInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_VulkanData.device, &layoutCreateInfo, nullptr,
                                        &m_VulkanData.computeDescriptorSetLayout) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute descriptor set layout!");
        }
    }

    void VulkanRenderer::createGraphicsPipeline()
    {
        // auto vertShaderCode = readFile("../../resources/shaders/vert.spv");
        // auto fragShaderCode = readFile("../../resources/shaders/frag.spv");

        std::vector<char> vertShaderCode;
        std::vector<char> fragShaderCode;

        auto entities = m_EntityManager->GetAllEntities();
        for (const auto &entity : entities)
        {
            std::shared_ptr<ShaderComponent> shaderComponent = entity->GetComponent<ShaderComponent>();
            vertShaderCode = shaderComponent->GetVertexShader()->GetShaderCode();
            fragShaderCode = shaderComponent->GetFragmentShader()->GetShaderCode();
        }

        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                          fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();
        vertexInputInfo.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());

        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
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
        multisampling.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable =
            VK_TRUE; // enable sample shading in the pipeline
        multisampling.minSampleShading =
            .2f; // min fraction for sample shading; closer to one is smoother
        multisampling.rasterizationSamples = m_VulkanData.msaaSamples;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
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
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                     VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_VulkanData.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(m_VulkanData.device, &pipelineLayoutInfo, nullptr,
                                   &m_VulkanData.pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

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

        pipelineInfo.layout = m_VulkanData.pipelineLayout;

        pipelineInfo.renderPass = m_VulkanData.renderPass;
        pipelineInfo.subpass = 0;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1;              // Optional

        if (vkCreateGraphicsPipelines(m_VulkanData.device, VK_NULL_HANDLE, 1,
                                      &pipelineInfo, nullptr,
                                      &m_VulkanData.graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_VulkanData.device, fragShaderModule, nullptr);
        vkDestroyShaderModule(m_VulkanData.device, vertShaderModule, nullptr);
    }

    void VulkanRenderer::createComputePipeline()
    {
        auto computeShaderCode = readFile("../../resources/shaders/particlecompute.spv");

        VkShaderModule computeShaderModule = createShaderModule(computeShaderCode);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_VulkanData.computeDescriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Pointer to push constant ranges

        if (vkCreatePipelineLayout(m_VulkanData.device, &pipelineLayoutInfo, nullptr,
                                   &m_VulkanData.computePipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkComputePipelineCreateInfo computePipelineInfo = {};
        computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computePipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computePipelineInfo.stage.module = computeShaderModule;
        computePipelineInfo.stage.pName = "main";
        computePipelineInfo.layout = m_VulkanData.computePipelineLayout;

        if (vkCreateComputePipelines(m_VulkanData.device, VK_NULL_HANDLE, 1,
                                     &computePipelineInfo, nullptr,
                                     &m_VulkanData.computePipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_VulkanData.device, computeShaderModule, nullptr);
    }

    void VulkanRenderer::createFramebuffer()
    {
        m_VulkanData.swapChainFramebuffers.resize(
            m_VulkanData.swapChainImageViews.size());
        for (size_t i = 0; i < m_VulkanData.swapChainImageViews.size(); i++)
        {
            std::array<VkImageView, 3> attachments = {
                m_VulkanData.colorImageView, m_VulkanData.depthImageView,
                m_VulkanData.swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_VulkanData.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_VulkanData.swapChainExtent.width;
            framebufferInfo.height = m_VulkanData.swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_VulkanData.device, &framebufferInfo, nullptr,
                                    &m_VulkanData.swapChainFramebuffers[i]) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanRenderer::createGUIFramebuffer()
    {
        m_VulkanData.swapChainGUIFramebuffers.resize(
            m_VulkanData.swapChainImageViews.size());
        for (size_t i = 0; i < m_VulkanData.swapChainImageViews.size(); i++)
        {
            std::array<VkImageView, 3> attachments = {
                m_VulkanData.colorImageView, m_VulkanData.depthImageView,
                m_VulkanData.swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_VulkanData.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_VulkanData.swapChainExtent.width;
            framebufferInfo.height = m_VulkanData.swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_VulkanData.device, &framebufferInfo, nullptr,
                                    &m_VulkanData.swapChainGUIFramebuffers[i]) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanRenderer::createCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices =
            findQueueFamilies(m_VulkanData.physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        if (vkCreateCommandPool(m_VulkanData.device, &poolInfo, nullptr,
                                &m_VulkanData.commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VulkanRenderer::createComputeCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices =
            findQueueFamilies(m_VulkanData.physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();

        if (vkCreateCommandPool(m_VulkanData.device, &poolInfo, nullptr,
                                &m_VulkanData.computeCommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create compute command pool!");
        }
    }

    void VulkanRenderer::createColorResources()
    {
        VkFormat colorFormat = m_VulkanData.swapChainImageFormat;

        createImage(m_VulkanData.swapChainExtent.width,
                    m_VulkanData.swapChainExtent.height, 1, m_VulkanData.msaaSamples,
                    colorFormat, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VulkanData.colorImage,
                    m_VulkanData.colorImageMemory);
        m_VulkanData.colorImageView = createImageView(
            m_VulkanData.colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void VulkanRenderer::createDepthResources()
    {
        VkFormat depthFormat = findDepthFormat();
        createImage(m_VulkanData.swapChainExtent.width,
                    m_VulkanData.swapChainExtent.height, 1, m_VulkanData.msaaSamples,
                    depthFormat, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VulkanData.depthImage,
                    m_VulkanData.depthImageMemory);
        m_VulkanData.depthImageView = createImageView(
            m_VulkanData.depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        transitionImageLayout(m_VulkanData.depthImage, depthFormat,
                              VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

    void VulkanRenderer::createTextureImage()
    {

        // int texWidth, texHeight, texChannels;
        // stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight,
        //     &texChannels, STBI_rgb_alpha);
        // VkDeviceSize imageSize = texWidth * texHeight * 4;
        // m_VulkanData.mipLevels = static_cast<uint32_t>(std::floor(
        //     std::log2(std::max(texWidth, texHeight)))) +
        //     1;
        // if (!pixels)
        //     {
        //     throw std::runtime_error("failed to load texture image!");
        //     }

        auto entities = m_EntityManager->GetAllEntities();
        std::shared_ptr<const TextureData> texture_data;
        for (const auto &entity : entities)
        {
            texture_data = entity->GetComponent<TextureComponent>()->GetTextureData();
        }
        int texWidth = texture_data->width;
        int texHeight = texture_data->height;
        m_VulkanData.mipLevels = static_cast<uint32_t>(std::floor(
                                     std::log2(std::max(texWidth, texHeight)))) +
                                 1;
        VkDeviceSize imageSize = texture_data->width * texture_data->height * 4;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        VulkanBuffer vulkanbuffer(&m_VulkanData);
        vulkanbuffer.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  stagingBuffer, stagingBufferMemory);
        void *data;
        vkMapMemory(m_VulkanData.device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, texture_data->pixels.get(), static_cast<size_t>(imageSize));
        vkUnmapMemory(m_VulkanData.device, stagingBufferMemory);

        // stbi_image_free(pixels);

        createImage(texture_data->width, texture_data->height, m_VulkanData.mipLevels,
                    VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VulkanData.textureImage,
                    m_VulkanData.textureImageMemory);

        transitionImageLayout(m_VulkanData.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              m_VulkanData.mipLevels);
        copyBufferToImage(stagingBuffer, m_VulkanData.textureImage,
                          static_cast<uint32_t>(texWidth),
                          static_cast<uint32_t>(texHeight));

        vkDestroyBuffer(m_VulkanData.device, stagingBuffer, nullptr);
        vkFreeMemory(m_VulkanData.device, stagingBufferMemory, nullptr);

        generateMipmaps(m_VulkanData.textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth,
                        texHeight, m_VulkanData.mipLevels);

        createTextureImageView();
        createTextureSampler();

        createDescriptorPool();
        createDescriptorSets();
    }

    void VulkanRenderer::createTextureImageView()
    {
        m_VulkanData.textureImageView =
            createImageView(m_VulkanData.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
                            VK_IMAGE_ASPECT_COLOR_BIT, m_VulkanData.mipLevels);
    }

    void VulkanRenderer::createTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;

        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_VulkanData.physicalDevice, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(m_VulkanData.mipLevels);

        if (vkCreateSampler(m_VulkanData.device, &samplerInfo, nullptr,
                            &m_VulkanData.textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void VulkanRenderer::loadModel()
    {
        auto entities = m_EntityManager->GetAllEntities();

        for (const auto &entity : entities)
        {
            auto model_data = entity->GetComponent<ModelComponent>()->GetModelData();
            for (int i = 0; i < model_data->indices.size(); i++)
            {

                Vertex vertex{};
                vertex.pos = model_data->positions[i];
                vertex.texCoord = model_data->uvs[i];
                vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
                vertices.push_back(vertex);
            }
            indices = model_data->indices;
        }
        createVertexBuffer();
        createIndexBuffer();
        // tinyobj::attrib_t attrib;
        // std::vector<tinyobj::shape_t> shapes;
        // std::vector<tinyobj::material_t> materials;
        // std::string warn, err;

        // if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
        //     MODEL_PATH.c_str()))
        //     {
        //     throw std::runtime_error(warn + err);
        //     }

        // std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        // for (const auto& shape : shapes)
        //     {
        //     for (const auto& index : shape.mesh.indices)
        //         {
        //         Vertex vertex{};

        //         vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
        //                       attrib.vertices[3 * index.vertex_index + 1],
        //                       attrib.vertices[3 * index.vertex_index + 2] };

        //         vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
        //                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

        //         vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

        //         if (uniqueVertices.count(vertex) == 0)
        //             {
        //             uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        //             vertices.push_back(vertex);
        //             }

        //         indices.push_back(uniqueVertices[vertex]);
        //         }
        //     }
    }

    void VulkanRenderer::createVertexBuffer()
    {

        m_VertexBuffer = std::unique_ptr<VulkanVertexBuffer>(
            VulkanVertexBuffer::Create(&m_VulkanData, vertices));
        // m_VertexBuffer->Bind();
    }

    void VulkanRenderer::createIndexBuffer()
    {
        m_IndexBuffer = std::unique_ptr<VulkanIndexBuffer>(
            VulkanIndexBuffer::Create(&m_VulkanData, indices));
    }

    void VulkanRenderer::createUniformBuffers()
    {
        m_UniformBuffers = std::unique_ptr<VulkanUniformBuffer>(
            VulkanUniformBuffer::Create(&m_VulkanData, MAX_FRAMES_IN_FLIGHT));
    }

    void VulkanRenderer::createShaderStorageBuffers(VkDeviceSize buffersize)
    {

        m_ShaderStorageBuffers = std::unique_ptr<VulkanBuffer>(
            VulkanShaderStorageBuffer<glm::mat4>::Create(&m_VulkanData, MAX_FRAMES_IN_FLIGHT, buffersize));
    }

    void VulkanRenderer::createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 4> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[3].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(m_VulkanData.device, &poolInfo, nullptr,
                                   &m_VulkanData.descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VulkanRenderer::createDescriptorSets(std::shared_ptr<Entity> entity, VulkanTextureData vulkanTextureData)
    {

        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                                   m_VulkanData.descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_VulkanData.descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        m_VulkanData.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        std::vector<VkDescriptorSet> descriptorsets;
        descriptorsets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_VulkanData.device, &allocInfo,
                                     descriptorsets.data()) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            // VkDescriptorBufferInfo bufferInfo{};
            // bufferInfo.buffer = m_UniformBuffers->GetUniformBuffers()[i];
            // bufferInfo.offset = 0;
            // bufferInfo.range = sizeof(UniformBufferObject);
            VkDescriptorBufferInfo cameraBufferInfo{};
            cameraBufferInfo.buffer = m_UniformBuffers->GetCameraBuffers()[i];
            cameraBufferInfo.offset = 0;
            cameraBufferInfo.range = sizeof(VulkanUniformBuffer::CameraUBO);

            VkDescriptorBufferInfo LightBufferInfo{};
            LightBufferInfo.buffer = m_UniformBuffers->GetLightBuffers()[i];
            LightBufferInfo.offset = 0;
            LightBufferInfo.range = sizeof(VulkanUniformBuffer::LightUBO);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = vulkanTextureData.textureImageView;
            imageInfo.sampler = vulkanTextureData.textureImageSampler;

            VkDescriptorBufferInfo ssboInfo{};
            ssboInfo.buffer = dynamic_cast<VulkanShaderStorageBuffer<glm::mat4> *>(m_ShaderStorageBuffers.get())->GetShaderStorageBuffers()[i];
            ssboInfo.offset = 0;
            ssboInfo.range = VK_WHOLE_SIZE;

            std::array<VkWriteDescriptorSet, 4> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorsets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &cameraBufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorsets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &LightBufferInfo;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = descriptorsets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pImageInfo = &imageInfo;

            descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[3].dstSet = descriptorsets[i];
            descriptorWrites[3].dstBinding = 3;
            descriptorWrites[3].dstArrayElement = 0;
            descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[3].descriptorCount = 1;
            descriptorWrites[3].pBufferInfo = &ssboInfo;

            vkUpdateDescriptorSets(m_VulkanData.device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
        }
        entityDescriptorSets[entity] = descriptorsets;
    }

    void VulkanRenderer::createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT,
                                                   m_VulkanData.descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_VulkanData.descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        m_VulkanData.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_VulkanData.device, &allocInfo,
                                     m_VulkanData.descriptorSets.data()) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_UniformBuffers->GetUniformBuffers()[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_VulkanData.textureImageView;
            imageInfo.sampler = m_VulkanData.textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_VulkanData.descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_VulkanData.descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(m_VulkanData.device,
                                   static_cast<uint32_t>(descriptorWrites.size()),
                                   descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanRenderer::createCommandBuffer()
    {
        m_VulkanData.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_VulkanData.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount =
            static_cast<uint32_t>(m_VulkanData.commandBuffers.size());

        if (vkAllocateCommandBuffers(m_VulkanData.device, &allocInfo,
                                     m_VulkanData.commandBuffers.data()) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
        ENGINE_INFO("createCommandBuffer success");
    }

    void VulkanRenderer::createComputeCommandBuffer()
    {
        m_VulkanData.computeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_VulkanData.computeCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount =
            static_cast<uint32_t>(m_VulkanData.computeCommandBuffers.size());

        if (vkAllocateCommandBuffers(m_VulkanData.device, &allocInfo,
                                     m_VulkanData.commandBuffers.data()) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate compute command buffers!");
        }
        ENGINE_INFO("createComputeCommandBuffer success");
    }

    void VulkanRenderer::createSyncObjects()
    {
        m_VulkanData.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_VulkanData.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_VulkanData.computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_VulkanData.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(m_VulkanData.device, &semaphoreInfo, nullptr,
                                  &m_VulkanData.imageAvailableSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(m_VulkanData.device, &semaphoreInfo, nullptr,
                                  &m_VulkanData.renderFinishedSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(m_VulkanData.device, &semaphoreInfo, nullptr,
                                  &m_VulkanData.computeFinishedSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateFence(m_VulkanData.device, &fenceInfo, nullptr,
                              &m_VulkanData.inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error(
                    "failed to create synchronization objects for a frame!");
            }
        }
        ENGINE_INFO("createSyncObject success");
    }

    void VulkanRenderer::cleanupSwapChain()
    {
        vkDestroyImageView(m_VulkanData.device, m_VulkanData.colorImageView, nullptr);
        vkDestroyImage(m_VulkanData.device, m_VulkanData.colorImage, nullptr);
        vkFreeMemory(m_VulkanData.device, m_VulkanData.colorImageMemory, nullptr);

        vkDestroyImageView(m_VulkanData.device, m_VulkanData.depthImageView, nullptr);
        vkDestroyImage(m_VulkanData.device, m_VulkanData.depthImage, nullptr);
        vkFreeMemory(m_VulkanData.device, m_VulkanData.depthImageMemory, nullptr);

        for (auto swapChainFramebuffer : m_VulkanData.swapChainFramebuffers)
        {
            vkDestroyFramebuffer(m_VulkanData.device, swapChainFramebuffer, nullptr);
        }

        for (auto swapChainImageView : m_VulkanData.swapChainImageViews)
        {
            vkDestroyImageView(m_VulkanData.device, swapChainImageView, nullptr);
        }

        vkDestroySwapchainKHR(m_VulkanData.device, m_VulkanData.swapChain, nullptr);
    }

    /*
    =============================================
    Helper Functions
    =============================================
    */

    std::vector<const char *> VulkanRenderer::getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions,
                                             glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool VulkanRenderer::isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;

        if (extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() &&
                                !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate &&
               supportedFeatures.samplerAnisotropy;
    };

    bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                             nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                             availableExtensions.data());

        std::set<std::string> requiredExtensions(
            m_VulkanData.deviceExtensions.begin(),
            m_VulkanData.deviceExtensions.end());

        for (const auto &extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    VulkanRenderer::QueueFamilyIndices
    VulkanRenderer::findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                                 queueFamilies.data());
        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                // Prefer a separate compute queue, but fall back to the same as graphics queue if necessary
                if (!indices.computeFamily.has_value() || !(queueFamilies[indices.computeFamily.value()].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                {
                    indices.computeFamily = i;
                }
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_VulkanData.surface,
                                                 &presentSupport);

            if (presentSupport)
            {
                indices.presentFamily = i;
            }
            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        return indices;
    }

    VulkanRenderer::SwapChainSupportDetails
    VulkanRenderer::querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_VulkanData.surface,
                                                  &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VulkanData.surface,
                                             &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_VulkanData.surface,
                                                 &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VulkanData.surface,
                                                  &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_VulkanData.surface,
                                                      &presentModeCount,
                                                      details.presentModes.data());
        }

        return details;
    }
    VkSurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto &availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR VulkanRenderer::chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes)
    {
        for (const auto &availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D
    VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        if (capabilities.currentExtent.width !=
            std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(m_VulkanData.window, &width, &height);

            VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                       static_cast<uint32_t>(height)};

            actualExtent.width =
                std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                           capabilities.maxImageExtent.width);
            actualExtent.height =
                std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                           capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    std::vector<char> VulkanRenderer::readFile(const std::string_view filename)
    {
        std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
    VkShaderModule
    VulkanRenderer::createShaderModule(const std::vector<char> &code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_VulkanData.device, &createInfo, nullptr,
                                 &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    void VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                             uint32_t imageIndex)
    {
        // VkCommandBufferBeginInfo beginInfo{};
        // beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // beginInfo.flags = 0;                  // Optional
        // beginInfo.pInheritanceInfo = nullptr; // Optional

        // if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        //     {
        //     throw std::runtime_error("failed to begin recording command buffer!");
        //     }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_VulkanData.renderPass;
        renderPassInfo.framebuffer = m_VulkanData.swapChainFramebuffers[imageIndex];

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_VulkanData.swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);

        auto entities = m_EntityManager->GetAllEntities();

        int cnt = 0;
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m_VulkanData.swapChainExtent.width);
        viewport.height = static_cast<float>(m_VulkanData.swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_VulkanData.swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        VkDeviceSize offsets[] = {0};
        VkBuffer vertexBuffers[] = {m_VertexBuffer->GetVertexBuffer()};
        VkBuffer indexBuffers = m_IndexBuffer->GetIndexBuffer();
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffers, 0, VK_INDEX_TYPE_UINT32);
        for (const auto &entity : entities)
        {
            // if (cnt == 0)
            // {
            //     cnt++;
            //     continue;
            // }

            auto entityBufferInfo_it = entityBufferInfos.find(entity);
            if (entityBufferInfo_it == entityBufferInfos.end())
                continue;
            auto entityPipelineInfo_it = entityPipelines.find(entity);
            if (entityPipelineInfo_it == entityPipelines.end())
                continue;
            // ENGINE_TRACE("render all entity");

            const EntityBufferInfo &bufferInfo = entityBufferInfo_it->second;
            const PipelineData &pipelineData = entityPipelineInfo_it->second;

            // ENGINE_TRACE("Rendering entity with vertexOffset: {0}, indexOffset: {1}, vertexCount: {2}, indexCount: {3}",
            //     bufferInfo.vertexOffset, bufferInfo.indexOffset, bufferInfo.vertexCount, bufferInfo.indexCount);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData.graphicsPipeline);

            // VkDeviceSize offsets[] = { 0 };
            VkDeviceSize vertexOffset = bufferInfo.vertexOffset * sizeof(Vertex);             // Assuming Vertex is the type of your vertex
            VkDeviceSize indexOffset = bufferInfo.indexOffset * sizeof(VK_INDEX_TYPE_UINT32); // Assuming uint32_t for index type

            ENGINE_TRACE("Vertex Offset: {}", bufferInfo.vertexOffset);
            ENGINE_TRACE("Index Offset: {}", bufferInfo.indexOffset);
            ENGINE_TRACE("Vertex Buffer Size: {}", m_VertexBuffer->GetBufferSize());
            ENGINE_TRACE("Index Buffer Size: {}", m_IndexBuffer->GetBufferSize());
            ENGINE_TRACE("Index Count: {}", bufferInfo.indexCount);

            // Check if the offsets are valid
            if (vertexOffset >= m_VertexBuffer->GetBufferSize() || indexOffset >= m_IndexBuffer->GetBufferSize())
            {
                ENGINE_ERROR("Vertex or Index offset is beyond buffer size");
                continue;
            }

            // vkCmdBindDescriptorSets(
            //     commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            //     pipelineData.pipelineLayout, 0, 1,
            //     &m_VulkanData.descriptorSets[m_VulkanData.currentFrame], 0, nullptr);

            vkCmdBindDescriptorSets(
                commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineData.pipelineLayout, 0, 1,
                &entityDescriptorSets[entity][m_VulkanData.currentFrame], 0, nullptr);

            // vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0,
            //     0, 0);
            vkCmdDrawIndexed(commandBuffer, bufferInfo.indexCount, 1, bufferInfo.indexOffset,
                             bufferInfo.vertexOffset, cnt);
            // vkCmdDrawIndexed(commandBuffer, bufferInfo.indexCount, 1, bufferInfo.indexOffset, 0, 0);
            // vkCmdDrawIndexed(commandBuffer, bufferInfo.indexCount, 1, 0, 0, 0);
            cnt++;
        }

        // vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        //     m_VulkanData.graphicsPipeline);

        // VkViewport viewport{};
        // viewport.x = 0.0f;
        // viewport.y = 0.0f;
        // viewport.width = static_cast<float>(m_VulkanData.swapChainExtent.width);
        // viewport.height = static_cast<float>(m_VulkanData.swapChainExtent.height);
        // viewport.minDepth = 0.0f;
        // viewport.maxDepth = 1.0f;
        // vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        // VkRect2D scissor{};
        // scissor.offset = { 0, 0 };
        // scissor.extent = m_VulkanData.swapChainExtent;
        // vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        // VkBuffer vertexBuffers[] = { m_VertexBuffer->GetVertexBuffer() };
        // VkBuffer indexBuffers = m_IndexBuffer->GetIndexBuffer();
        // VkDeviceSize offsets[] = { 0 };
        // vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        // vkCmdBindIndexBuffer(commandBuffer, indexBuffers, 0, VK_INDEX_TYPE_UINT32);

        // vkCmdBindDescriptorSets(
        //     commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        //     m_VulkanData.pipelineLayout, 0, 1,
        //     &m_VulkanData.descriptorSets[m_VulkanData.currentFrame], 0, nullptr);
        // vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0,
        //     0, 0);

        // vkCmdEndRenderPass(commandBuffer);

        // if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        //     {
        //     throw std::runtime_error("failed to record command buffer!");
        //     }
    }

    void VulkanRenderer::createImage(uint32_t width, uint32_t height,
                                     uint32_t mipLevels,
                                     VkSampleCountFlagBits numSamples,
                                     VkFormat format, VkImageTiling tiling,
                                     VkImageUsageFlags usage,
                                     VkMemoryPropertyFlags properties,
                                     VkImage &image, VkDeviceMemory &imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(m_VulkanData.device, &imageInfo, nullptr, &image) !=
            VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_VulkanData.device, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        VulkanBuffer vulkanbuffer(&m_VulkanData);
        allocInfo.memoryTypeIndex =
            vulkanbuffer.findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_VulkanData.device, &allocInfo, nullptr,
                             &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(m_VulkanData.device, image, imageMemory, 0);
    }

    void VulkanRenderer::transitionImageLayout(VkImage image, VkFormat format,
                                               VkImageLayout oldLayout,
                                               VkImageLayout newLayout,
                                               uint32_t mipLevels)
    {
        VulkanBuffer vulkanbuffer(&m_VulkanData);
        VkCommandBuffer commandBuffer = vulkanbuffer.beginSingleTimeCommands();
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
            newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                 newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);

        vulkanbuffer.endSingleTimeCommands(commandBuffer);
    }

    void VulkanRenderer::copyBufferToImage(VkBuffer buffer, VkImage image,
                                           uint32_t width, uint32_t height)
    {
        VulkanBuffer vulkanbuffer(&m_VulkanData);
        VkCommandBuffer commandBuffer = vulkanbuffer.beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        vulkanbuffer.endSingleTimeCommands(commandBuffer);
    }

    VkFormat
    VulkanRenderer::findSupportedFormat(const std::vector<VkFormat> &candidates,
                                        VkImageTiling tiling,
                                        VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_VulkanData.physicalDevice, format,
                                                &props);

            if (tiling == VK_IMAGE_TILING_LINEAR &&
                (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                     (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat VulkanRenderer::findDepthFormat()
    {
        return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
             VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    bool VulkanRenderer::hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
               format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void VulkanRenderer::generateMipmaps(VkImage image, VkFormat imageFormat,
                                         int32_t texWidth, int32_t texHeight,
                                         uint32_t mipLevels)
    {
        VulkanBuffer vulkanbuffer(&m_VulkanData);
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(m_VulkanData.physicalDevice, imageFormat,
                                            &formatProperties);

        if (!(formatProperties.optimalTilingFeatures &
              VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error(
                "texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = vulkanbuffer.beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                                 nullptr, 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1,
                                  mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                                 0, nullptr, 1, &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                             nullptr, 1, &barrier);

        vulkanbuffer.endSingleTimeCommands(commandBuffer);
    }

    VkSampleCountFlagBits VulkanRenderer::getMaxUsableSampleCount()
    {
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(m_VulkanData.physicalDevice,
                                      &physicalDeviceProperties);

        VkSampleCountFlags counts =
            physicalDeviceProperties.limits.framebufferColorSampleCounts &
            physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }

} // namespace Engine
