#pragma once
#include "AssetData.hpp"
#include <string>
#include <vector>

namespace Engine {
    class ShaderData : public AssetData {
        public:
        ShaderData();
        virtual ~ShaderData();
        bool Load(const std::string& path) override;

        const std::vector<char>& GetShaderCode() const;

        protected:
        std::vector<char> shaderCode;
        };

    class VertexShaderData : public ShaderData {
        public:
        bool Load(const std::string& path) override;
        };

    class FragmentShaderData : public ShaderData {
        public:
        bool Load(const std::string& path) override;
        };

    class ComputeShaderData : public ShaderData {
        public:
        bool Load(const std::string& path) override;
        };
    }
