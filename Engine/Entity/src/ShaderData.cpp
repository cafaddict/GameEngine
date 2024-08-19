#include "ShaderData.hpp"

namespace Engine {
    ShaderData::ShaderData() {}

    ShaderData::~ShaderData() {}

    bool ShaderData::Load(const std::string& path) {
        ShaderData::shaderCode = AssetData::readFile(path);
        return true;
        }

    const std::vector<char>& ShaderData::GetShaderCode() const {
        return shaderCode;
        }

    bool VertexShaderData::Load(const std::string& path) {
        return ShaderData::Load(path);
        }

    bool FragmentShaderData::Load(const std::string& path) {
        return ShaderData::Load(path);
        }

    bool ComputeShaderData::Load(const std::string& path) {
        return ShaderData::Load(path);
        }
    }
