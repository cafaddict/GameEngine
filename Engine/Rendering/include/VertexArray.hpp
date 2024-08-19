#pragma once
#include <vulkan/vulkan.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>
// #include <span>
#include <array>

namespace Engine {

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec4 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
            }
        // This shoud be changed support arbitrary number of attributes
        static std::array<VkVertexInputAttributeDescription, 4>
            getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, color);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, texCoord);
            return attributeDescriptions;
            }

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color &&
                texCoord == other.texCoord && normal == other.normal;
            }
        };

    class VertexArray {
        public:
        VertexArray(std::vector<Vertex> vertices);
        unsigned int getAttributeNum();
        inline size_t getVerticesSize();

        private:
        std::vector<Vertex> m_Vertices;
        unsigned int m_Attribute_num;
        };
    }  // namespace Engine

namespace std {
    template <>
    struct hash<Engine::Vertex> {
        size_t operator()(Engine::Vertex const& vertex) const {
            size_t seed = 0;
            hash<glm::vec3> vec3Hasher;
            hash<glm::vec2> vec2Hasher;

            seed ^= vec3Hasher(vertex.pos) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= vec3Hasher(vertex.color) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= vec3Hasher(vertex.normal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= vec2Hasher(vertex.texCoord) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
            }
        };
    }