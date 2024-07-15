#include "TextureData.hpp"
#include <stb_image/stb_image.h>


namespace Engine {

    TextureData::TextureData() {

        }
    TextureData::~TextureData() {
        free(this->pixels);
        }

    bool TextureData::Load(const std::string& texturePath) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixelData = stbi_load(texturePath.c_str(), &texWidth, &texHeight,
            &texChannels, STBI_rgb_alpha);
        size_t imageSize = texWidth * texHeight * 4;

        if (!pixelData)
            {
            ENGINE_ERROR("Failed to load texture");
            return false;
            }

        this->width = texWidth;
        this->height = texHeight;
        this->channels = texChannels;




        this->pixels = malloc(imageSize);

        std::memcpy(this->pixels, pixelData, imageSize);


        stbi_image_free(pixelData);

        return true;
        }

    }