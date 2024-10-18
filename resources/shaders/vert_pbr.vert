#version 410

// UBO for camera matrices
layout(std140) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
};

// UBO for lighting parameters
layout(std140) uniform LightUBO {
    vec3 lightPos;
    vec3 lightColor;
    float lightIntensity;
};

// UBO for transformation matrices (fixed size as GLSL 410 does not support SSBOs)
layout(std140) uniform TransformUBO {
    mat4 transformations[100]; // Adjust size based on your needs and hardware limits
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec4 fragColor;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 fragTangent;
layout(location = 5) out vec3 fragBitangent;

void main() {
    uint instanceIndex = gl_InstanceID; // Use gl_InstanceID in GLSL 410
    mat4 model = transformations[instanceIndex];

    fragPos = vec3(model * vec4(inPosition, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(model))) * inNormal);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragTangent = mat3(model) * inTangent;
    fragBitangent = cross(fragNormal, fragTangent);

    gl_Position = proj * view * vec4(fragPos, 1.0);
}
