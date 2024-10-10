#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>
#include <sstream>

namespace Engine {
class AssetData {
    public:
    virtual ~AssetData() = default;
    virtual bool Load(const std::string &path) = 0;

    static std::vector<char> readFile(const std::string_view filename) {
        // Open the file in binary mode, and position the file pointer at the end
        std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(filename));
        }

        // Get the file size
        size_t fileSize = static_cast<size_t>(file.tellg());

        // Create a buffer to hold the file contents
        std::vector<char> buffer(fileSize);

        // Rewind to the beginning of the file and read its contents into the buffer
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        // Check if the read was successful
        if (!file) {
            throw std::runtime_error("Error reading file: " + std::string(filename));
        }

        file.close();

        return buffer;
    }

    static std::vector<char> readShaderFile(const std::string_view filename) {
        // Open the file in text mode and start reading from the beginning
        std::ifstream file(filename.data());
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(filename));
        }

        // Read the entire file into a string using string stream
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        std::cout << buffer.str() << std::endl;
        return std::vector<char>(buffer.str().begin(), buffer.str().end());
    }
};
} // namespace Engine
