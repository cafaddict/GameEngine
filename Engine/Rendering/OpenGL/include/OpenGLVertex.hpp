#pragma once
#include "glm/fwd.hpp"
#include <glm/glm.hpp>
struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texCoord;
    glm::vec3 tangent;
};