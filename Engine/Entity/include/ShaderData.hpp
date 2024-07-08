#pragma once
#include "AssetData.hpp"
#include <string>
#include <vector>

namespace Engine {
    class ShaderData : public AssetData {
        public:
        ShaderData();
        ~ShaderData() override;
        bool Load(const std::string& path) override;
        bool LoadVertexShader(const std::string& path);
        bool LoadFragShader(const std::string& path);
        bool LoadComputeShader(const std::string& path);

        std::vector<char> vertexShaderCode;
        std::vector<char> fragmentShaderCode;
        std::vector<char> computeShaderCode;
        };
    }
