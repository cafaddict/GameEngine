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
            glm::vec3 position;
            glm::vec3 velocity;
            glm::vec4 color;

            public:
            Particle(/* args */);
            ~Particle();
            glm::vec3 getVelocity();
            glm::vec3 getPosition();
            glm::vec4 getColor();
        };

    Particle::Particle(/* args */)
        {
        }

    Particle::~Particle()
        {
        }


    } // namespace Engine
