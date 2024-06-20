#include <Particle.hpp>
namespace Engine {
    glm::vec3 Particle::getVelocity()
        {
        return velocity;
        }

    glm::vec3 Particle::getPosition()
        {
        return position;
        }

    glm::vec4 Particle::getColor()
        {
        return color;
        }

    }