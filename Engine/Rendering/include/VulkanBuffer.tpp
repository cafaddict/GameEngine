namespace Engine {
    template <typename T> VulkanShaderStorageBuffer<T>::~VulkanShaderStorageBuffer() {}
    template <typename T> VulkanShaderStorageBuffer<T>::VulkanShaderStorageBuffer() {}

    template <typename T> VulkanShaderStorageBuffer<T>::VulkanShaderStorageBuffer(VulkanData* vulkanData,
        int MAX_FRAMES_IN_FLIGHT, VkDeviceSize buffersize, std::vector<T> objects)
        {
        m_Device = &vulkanData->device;
        m_PhysicalDevice = &vulkanData->physicalDevice;
        m_CommandPool = &vulkanData->computeCommandPool;
        m_GraphicsQueue = &vulkanData->graphicsQueue;
        m_max_frame_in_flight = MAX_FRAMES_IN_FLIGHT;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(buffersize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
        void* data;
        vkMapMemory(*m_Device, stagingBufferMemory, 0, buffersize, 0, &data);
        memcpy(data, objects.data(), (size_t)buffersize);
        vkUnmapMemory(*m_Device, stagingBufferMemory);


        m_ShaderStorageBuffers.resize(m_max_frame_in_flight);
        m_ShaderStorageBuffersMapped.resize(m_max_frame_in_flight);
        m_ShaderStorageBuffersMemory.resize(m_max_frame_in_flight);

        for (size_t i = 0; i < m_max_frame_in_flight; i++)
            {
            createBuffer(buffersize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                m_ShaderStorageBuffers[i], m_ShaderStorageBuffersMemory[i]);
            copyBuffer(stagingBuffer, m_ShaderStorageBuffers[i], buffersize);
            }

        vkDestroyBuffer(*m_Device, stagingBuffer, nullptr);
        vkFreeMemory(*m_Device, stagingBufferMemory, nullptr);
        }
    template <typename T>
    void VulkanShaderStorageBuffer<T>::Bind() {}
    template <typename T>
    void VulkanShaderStorageBuffer<T>::UnBind() {}
    template <typename T>
    void VulkanShaderStorageBuffer<T>::SetData(const void* data, uint32_t size) { data; size; };
    template <typename T> VulkanShaderStorageBuffer<T>* VulkanShaderStorageBuffer<T>::Create(VulkanData* vulkanData,
        int MAX_FRAMES_IN_FLIGHT, VkDeviceSize buffersize, std::vector<T> objects)
        {
        return new VulkanShaderStorageBuffer<T>(vulkanData, MAX_FRAMES_IN_FLIGHT, buffersize, objects);
        }
    template <typename T>
    void VulkanShaderStorageBuffer<T>::Destroy()
        {
        for (size_t i = 0; i < m_max_frame_in_flight; i++)
            {
            vkDestroyBuffer(*m_Device, m_ShaderStorageBuffers[i], nullptr);
            vkFreeMemory(*m_Device, m_ShaderStorageBuffersMemory[i], nullptr);
            }
        }
    }