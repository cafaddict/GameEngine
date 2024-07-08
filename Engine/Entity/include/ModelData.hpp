#pragma once
#include "AssetData.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
namespace Engine {
    class ModelData : public AssetData
        {
            private:

            public:
            ModelData();
            ~ModelData();
            bool Load(const std::string& path) override;
            std::vector <glm::vec3> positions;
            std::vector <glm::vec3> normals;
            std::vector<glm::vec2> uvs;
            std::vector<uint32_t> indices;
        };



    }