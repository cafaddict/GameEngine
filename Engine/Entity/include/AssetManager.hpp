#pragma once
#include "AssetData.hpp"
#include "Log.hpp"
#include "ModelData.hpp"
#include "ShaderData.hpp"
#include "TextureData.hpp"
#include <unordered_map>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
namespace Engine {
class AssetManager {
    struct pair_hash {
        template <class T1, class T2> std::size_t operator()(const std::pair<T1, T2> &pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    public:
    AssetManager(/* args */) {};
    ~AssetManager() {};
    template <typename T> std::shared_ptr<T> GetAsset(const std::string &path) {
        std::type_index typeIndex = std::type_index(typeid(T));
        auto key = std::make_pair(path, typeIndex);

        auto it = assetCache.find(key);
        if (it != assetCache.end()) {

            return std::dynamic_pointer_cast<T>(it->second);
        }
        std::shared_ptr<T> asset;
        if (typeIndex == std::type_index(typeid(TextureData))) {
            // Check if T is actually TextureData
            ENGINE_WARN("Graphics API: {0}", graphicsAPI);
            asset = std::dynamic_pointer_cast<T>(std::make_shared<TextureData>(graphicsAPI));

        } else {
            // Use the default constructor for other asset types
            asset = std::make_shared<T>();
        }
        if (asset->Load(path)) {

            assetCache[key] = asset;
            return asset;
        }

        return nullptr;
    }

    void SetGraphicsAPI(GraphicsAPI api) {
        ENGINE_WARN("Setting Graphics API: {0}", api);
        graphicsAPI = api;
    }

    private:
    std::unordered_map<std::pair<std::string, std::type_index>, std::shared_ptr<AssetData>, pair_hash> assetCache;
    GraphicsAPI graphicsAPI = GraphicsAPI::OpenGL;
};

}; // namespace Engine
