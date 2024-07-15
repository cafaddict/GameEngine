#pragma once
#include "AssetData.hpp"
#include <Log.hpp>


#include <chrono>
namespace Engine {
    class TextureData : public AssetData
        {
            private:

            public:
            TextureData();
            ~TextureData();
            bool Load(const std::string& path) override;
            void* pixels;
            int width;
            int height;
            int channels;


        };



    }