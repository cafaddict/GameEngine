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
        // Check that textureData is valid and contains data
        if (textureData && textureData->pixels) {
            GLenum format = (textureData->channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, textureData->width, textureData->height, 0, format, GL_UNSIGNED_BYTE,
                         textureData->pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cerr << "Invalid texture data!" << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }
    ~OpenGLTexture() { glDeleteTextures(1, &m_Texture); };
    GLuint GetTexture() const { return m_Texture; }

    private:
    GLuint m_Texture;
};
} // namespace Engine