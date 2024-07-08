#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace Engine {
    class AssetData {
        public:
        virtual ~AssetData() = default;
        virtual bool Load(const std::string& path) = 0;

        static std::vector<char> readFile(const std::string_view filename) {
            std::ifstream file(filename.data(), std::ios::ate | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("failed to open file!");
                }
            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();

            return buffer;
            }
        };
    }
