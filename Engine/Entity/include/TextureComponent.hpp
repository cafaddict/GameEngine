#pragma once
#include "Component.hpp"
#include "TextureData.hpp"
#include <string>
namespace Engine {
    class TextureComponent : public Component
        {
            private:
            std::shared_ptr<const TextureData> textureData;
            public:
            TextureComponent(std::shared_ptr<const TextureData> textureData);
            TextureComponent();
            ~TextureComponent();

            void SetTextureData(std::shared_ptr<const TextureData> textureData);
            std::shared_ptr<const TextureData> GetTextureData() const;

        };



    }