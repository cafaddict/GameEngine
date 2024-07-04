#pragma once
#include "Component.hpp"
namespace Engine {
    class ShaderComponent : public Component
        {
            private:
            /* data */
            public:
            ShaderComponent(/* args */);
            ~ShaderComponent();
        };

    ShaderComponent::ShaderComponent(/* args */)
        {
        }

    ShaderComponent::~ShaderComponent()
        {
        }

    }