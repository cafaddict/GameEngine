#include "TextureData.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Engine {

TextureData::TextureData() {}
TextureData::~TextureData() {
    // free(this->pixels);
}

bool TextureData::Load(const std::string &texturePath) {
    int texWidth, texHeight, texChannels;
    bool forceRGBA = (graphicsAPI == GraphicsAPI::Vulkan);
    stbi_uc *pixelData =
        stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, forceRGBA ? STBI_rgb_alpha : 0);
    size_t imageSize = texWidth * texHeight * 4;

    if (!pixelData) {
        ENGINE_ERROR("Failed to load texture");
        return false;
    }

    this->width = texWidth;
    this->height = texHeight;
    this->channels = texChannels;

    // this->pixels = malloc(imageSize);

    // std::memcpy(this->pixels, pixelData, imageSize);

    auto deleter = [](void *ptr) { stbi_image_free(ptr); };

    this->pixels = std::shared_ptr<void>(pixelData, deleter);

    // stbi_image_free(pixelData);

    return true;
}

} // namespace Engine