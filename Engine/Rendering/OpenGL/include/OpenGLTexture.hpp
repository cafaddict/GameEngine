#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <TextureComponent.hpp>
namespace Engine {
class OpenGLTexture {
    public:
    OpenGLTexture() = default;
    OpenGLTexture(std::shared_ptr<const TextureData> textureData) {
        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureData->width, textureData->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     textureData->pixels.get());
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    ~OpenGLTexture() { glDeleteTextures(1, &m_Texture); };
    GLuint GetTexture() const { return m_Texture; }

    private:
    GLuint m_Texture;
};
} // namespace Engine