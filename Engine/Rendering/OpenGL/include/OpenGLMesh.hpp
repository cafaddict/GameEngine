#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "OpenGLVertex.hpp"
#include "ModelComponent.hpp"
#include <vector>

namespace Engine {
class OpenGLMesh {
    public:
    OpenGLMesh() = default;
    OpenGLMesh(std::shared_ptr<const ModelData> modelData) {
        std::vector<Vertex> vertices;
        for (size_t i = 0; i < modelData->positions.size(); i++) {
            Vertex vertex;
            vertex.pos = modelData->positions[i];
            vertex.normal = modelData->normals.size() > i ? modelData->normals[i] : glm::vec3(0.0f, 0.0f, 1.0f);
            vertex.texCoord = modelData->uvs.size() > i ? modelData->uvs[i] : glm::vec2(0.0f);
            vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
            vertices.push_back(vertex);
        }
        m_IndexCount = static_cast<GLsizei>(modelData->indices.size());

        glGenVertexArrays(1, &m_VertexArray);
        glGenBuffers(1, &m_VertexBuffer);
        glGenBuffers(1, &m_IndexBuffer);

        glBindVertexArray(m_VertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData->indices.size() * sizeof(unsigned int),
                     modelData->indices.data(), GL_STATIC_DRAW);

        GLsizei stride = sizeof(Vertex);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, pos));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(3);

        // Unbind the VAO to prevent accidental changes
        glBindVertexArray(0);
    }
    ~OpenGLMesh() {
        glDeleteVertexArrays(1, &m_VertexArray);
        glDeleteBuffers(1, &m_VertexBuffer);
        glDeleteBuffers(1, &m_IndexBuffer);
    }

    GLuint GetVertexArray() const { return m_VertexArray; }
    GLuint GetIndexBuffer() const { return m_IndexBuffer; }
    GLuint GetVertexBuffer() const { return m_VertexBuffer; }
    GLsizei GetIndexCount() const { return m_IndexCount; }

    private:
    GLuint m_VertexArray;
    GLuint m_VertexBuffer;
    GLuint m_IndexBuffer;
    GLsizei m_IndexCount;
};
} // namespace Engine