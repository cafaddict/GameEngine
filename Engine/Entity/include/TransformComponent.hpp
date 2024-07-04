#pragma once
#include "Component.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Engine {
    class TransformComponent : public Component
        {
            private:
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
            public:
            TransformComponent(/* args */);
            ~TransformComponent();
        };

    }