#pragma once
#include "Component.hpp"
#include <string>
namespace Engine {
    class ModelComponent : public Component
        {
            private:
            std::string modelPath;
            public:
            ModelComponent(const std::string& path) : modelPath(path) {};
            ~ModelComponent();
        };



    }