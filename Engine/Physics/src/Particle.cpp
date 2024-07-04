#include <Particle.hpp>
namespace Engine {

    Particle::Particle() {

        }

    Particle::~Particle() {

        }

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