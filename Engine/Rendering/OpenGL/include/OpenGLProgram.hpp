#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Log.hpp"
#include <iostream>
#include <vector>

namespace Engine {
class OpenGLProgram {
    public:
    OpenGLProgram() {};
    OpenGLProgram(std::vector<char> vertexShaderCode, std::vector<char> fragmentShaderCode) {
        std::string vertexShaderString(vertexShaderCode.begin(), vertexShaderCode.end());

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char *vertexShaderCodePtr = vertexShaderString.c_str();

        glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
        glCompileShader(vertexShader);

        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            ENGINE_ERROR("Vertex shader compilation failed: {0}", infoLog);
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *fragmentShaderCodePtr = fragmentShaderCode.data();
        glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            ENGINE_ERROR("Fragment shader compilation failed: {0}", infoLog);
        }

        m_Program = glCreateProgram();
        glAttachShader(m_Program, vertexShader);
        glAttachShader(m_Program, fragmentShader);
        glLinkProgram(m_Program);

        glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_Program, 512, nullptr, infoLog);
            ENGINE_ERROR("Program linking failed: {0}", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    OpenGLProgram(std::vector<char> vertexShaderCode, std::vector<char> fragmentShaderCode,
                  std::vector<char> computeShaderCode) {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char *vertexShaderCodePtr = vertexShaderCode.data();
        glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
        glCompileShader(vertexShader);

        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            ENGINE_ERROR("Vertex shader compilation failed: {0}", infoLog);
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char *fragmentShaderCodePtr = fragmentShaderCode.data();
        glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            ENGINE_ERROR("Fragment shader compilation failed: {0}", infoLog);
        }
#ifdef GL_COMPUTE_SHADER
        GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
        const char *computeShaderCodePtr = computeShaderCode.data();
        glShaderSource(computeShader, 1, &computeShaderCodePtr, nullptr);
        glCompileShader(computeShader);

        glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
            ENGINE_ERROR("Compute shader compilation failed: {0}", infoLog);
        }
#endif
        m_Program = glCreateProgram();
        glAttachShader(m_Program, vertexShader);
        glAttachShader(m_Program, fragmentShader);
#ifdef GL_COMPUTE_SHADER
        glAttachShader(m_Program, computeShader);
#endif
        glLinkProgram(m_Program);

        glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(m_Program, 512, nullptr, infoLog);
            ENGINE_ERROR("Program linking failed: {0}", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
#ifdef GL_COMPUTE_SHADER
        glDeleteShader(computeShader);
#endif
    }
    ~OpenGLProgram() { glDeleteProgram(m_Program); };

    GLuint GetProgram() const { return m_Program; }

    private:
    GLuint m_Program;
};
} // namespace Engine