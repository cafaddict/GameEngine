#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Engine
    {
    class Particle
        {
            private:


            public:
            Particle(/* args */);
            ~Particle();
            glm::vec3 getVelocity();
            glm::vec3 getPosition();
            glm::vec4 getColor();
            glm::vec3 position;
            glm::vec3 velocity;
            glm::vec4 color;
        };



    } // namespace Engine
