#pragma once
#include "AssetData.hpp"
#include <Log.hpp>

#include <chrono>
namespace Engine {
class TextureData : public AssetData {
    private:
    GraphicsAPI graphicsAPI;

    public:
    TextureData();
    TextureData(GraphicsAPI api) : graphicsAPI(api) {}

    ~TextureData();
    bool Load(const std::string &path) override;
    std::shared_ptr<void> pixels;
    // void* pixels;
    int width;
    int height;
    int channels;
};

} // namespace Engine