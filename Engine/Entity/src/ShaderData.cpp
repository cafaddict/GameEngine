#include "ShaderData.hpp"

namespace Engine {
    ShaderData::ShaderData() {}

    ShaderData::~ShaderData() {}

    bool ShaderData::Load(const std::string& path) {
        // This method should implement some generic shader loading if needed
        return true;
        }

    bool ShaderData::LoadVertexShader(const std::string& path) {
        vertexShaderCode = AssetData::readFile(path);
        return true;
        }

    bool ShaderData::LoadFragShader(const std::string& path) {
        fragmentShaderCode = AssetData::readFile(path);
        return true;
        }

    bool ShaderData::LoadComputeShader(const std::string& path) {
        computeShaderCode = AssetData::readFile(path);
        return true;
        }
    }
