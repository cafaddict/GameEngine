#pragma once
#include "ModelData.hpp"
#include "ShaderData.hpp"
#include <unordered_map>
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
namespace Engine {
    class AssetManager
        {
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator() (const std::pair<T1, T2>& pair) const {
                return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
                }
            };

            public:
            AssetManager(/* args */) {};
            ~AssetManager() {};
            template<typename T>
            std::shared_ptr<T> GetAsset(const std::string& path) {
                std::type_index typeIndex = std::type_index(typeid(T));
                auto key = std::make_pair(path, typeIndex);

                auto it = assetCache.find(key);
                if (it != assetCache.end()) {
                    return std::dynamic_pointer_cast<T>(it->second);
                    }

                std::shared_ptr<T> asset = std::make_shared<T>();
                if (asset->Load(path)) {
                    assetCache[key] = asset;
                    return asset;
                    }

                return nullptr;
                }

            private:
            std::unordered_map<std::pair<std::string, std::type_index>, std::shared_ptr<AssetData>, pair_hash> assetCache;


        };

    };
