#include "TextureData.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace Engine {

TextureData::TextureData() {}
TextureData::~TextureData() {
    // free(this->pixels);
}

bool TextureData::Load(const std::string &texturePath) {
    // int texWidth, texHeight, texChannels;
    // stbi_uc *pixelData = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    // size_t imageSize = texWidth * texHeight * 4;

    // if (!pixelData) {
    //     ENGINE_ERROR("Failed to load texture");
    //     return false;
    // }

    // this->width = texWidth;
    // this->height = texHeight;
    // this->channels = texChannels;

    // // this->pixels = malloc(imageSize);

    // // std::memcpy(this->pixels, pixelData, imageSize);

    // auto deleter = [](void *ptr) { stbi_image_free(ptr); };

    // this->pixels = std::shared_ptr<void>(pixelData, deleter);

    // // stbi_image_free(pixelData);

    // return true;

    // Load image using stb_image
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << texturePath << std::endl;
        return false;
    }

    // Wrap the pixel data in a shared_ptr with a custom deleter
    pixels = std::shared_ptr<void>(data, stbi_image_free);

    return true;
}

} // namespace Engine