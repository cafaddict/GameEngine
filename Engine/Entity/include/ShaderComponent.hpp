#pragma once
#include "Component.hpp"
#include "ShaderData.hpp"
#include <memory>

namespace Engine {
    class ShaderComponent : public Component {
        private:
        std::shared_ptr<const VertexShaderData> vertexShader;
        std::shared_ptr<const FragmentShaderData> fragmentShader;
        std::shared_ptr<const ComputeShaderData> computeShader;

        public:
        ShaderComponent(
            std::shared_ptr<const VertexShaderData> vertexShader = nullptr,
            std::shared_ptr<const FragmentShaderData> fragmentShader = nullptr,
            std::shared_ptr<const ComputeShaderData> computeShader = nullptr);

        ~ShaderComponent();

        void SetVertexShader(std::shared_ptr<const VertexShaderData> vertexShader);
        void SetFragmentShader(std::shared_ptr<const FragmentShaderData> fragmentShader);
        void SetComputeShader(std::shared_ptr<const ComputeShaderData> computeShader);

        std::shared_ptr<const VertexShaderData> GetVertexShader() const;
        std::shared_ptr<const FragmentShaderData> GetFragmentShader() const;
        std::shared_ptr<const ComputeShaderData> GetComputeShader() const;
        };
    }
