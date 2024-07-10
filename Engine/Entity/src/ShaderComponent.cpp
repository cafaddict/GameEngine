#include "ShaderComponent.hpp"

namespace Engine {
    ShaderComponent::ShaderComponent(
        std::shared_ptr<const VertexShaderData> vertexShader,
        std::shared_ptr<const FragmentShaderData> fragmentShader,
        std::shared_ptr<const ComputeShaderData> computeShader) {
        ShaderComponent::vertexShader = vertexShader;
        ShaderComponent::fragmentShader = fragmentShader;
        ShaderComponent::computeShader = computeShader;
        }

    ShaderComponent::~ShaderComponent() {}

    void ShaderComponent::SetVertexShader(std::shared_ptr<const VertexShaderData> vertexShader) {
        this->vertexShader = vertexShader;
        }

    void ShaderComponent::SetFragmentShader(std::shared_ptr<const FragmentShaderData> fragmentShader) {
        this->fragmentShader = fragmentShader;
        }

    void ShaderComponent::SetComputeShader(std::shared_ptr<const ComputeShaderData> computeShader) {
        this->computeShader = computeShader;
        }

    std::shared_ptr<const VertexShaderData> ShaderComponent::GetVertexShader() const {
        return ShaderComponent::vertexShader;
        }

    std::shared_ptr<const FragmentShaderData> ShaderComponent::GetFragmentShader() const {
        return ShaderComponent::fragmentShader;
        }

    std::shared_ptr<const ComputeShaderData> ShaderComponent::GetComputeShader() const {
        return ShaderComponent::computeShader;
        }
    }
