#pragma once
#include "Component.hpp"
#include <string>
namespace Engine {
    class TextureComponent : public Component
        {
            private:
            std::string texturePath;
            public:
            TextureComponent(const std::string& path) : texturePath(path) {};
            ~TextureComponent();
        };



    }