#include "TextureComponent.hpp"

namespace Engine {
    TextureComponent::TextureComponent(std::shared_ptr<const TextureData> texture_data) {
        TextureComponent::textureData = texture_data;
        }
    TextureComponent::~TextureComponent() {

        }


    void TextureComponent::SetTextureData(std::shared_ptr<const TextureData> textureData) {
        this->textureData = textureData;
        }


    std::shared_ptr<const TextureData> TextureComponent::GetTextureData() const {
        return TextureComponent::textureData;
        }
    }