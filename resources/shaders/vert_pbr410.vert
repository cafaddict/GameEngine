#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

out vec2 fragUV;
out vec3 fragNormal;
out vec3 fragTangent;
out vec3 fragBitangent;
out vec3 fragWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragUV = inUV;

    // Transform normal, tangent, bitangent to world space
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    fragNormal = normalize(normalMatrix * inNormal);
    fragTangent = normalize(normalMatrix * inTangent);
    fragBitangent = normalize(normalMatrix * inBitangent);

    // World position
    fragWorldPos = vec3(model * vec4(inPosition, 1.0));

    gl_Position = projection * view * vec4(fragWorldPos, 1.0);
}
