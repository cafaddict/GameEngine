#version 450

// UBO for camera matrices
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
}
camera;

// UBO for lighting parameters
layout(set = 0, binding = 1) uniform LightUBO {
    vec3 lightPos;
    vec3 lightColor;
    float lightIntensity;
}
light;

// SSBO for transformation matrices
layout(set = 0, binding = 2) readonly buffer StorageBufferObject { mat4 transformations[]; }
ssbo;

// Input attributes from vertex buffer
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;

// Output to fragment shader
layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec4 fragColor;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 fragTangent;
layout(location = 5) out vec3 fragBitangent;

void main() {
    uint instanceIndex = gl_InstanceIndex; // Use gl_InstanceIndex in Vulkan (not gl_InstanceID)
    mat4 model = ssbo.transformations[instanceIndex];

    fragPos = vec3(model * vec4(inPosition, 1.0));
    fragNormal = normalize(mat3(transpose(inverse(model))) * inNormal);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragTangent = mat3(model) * inTangent;
    fragBitangent = cross(fragNormal, fragTangent);

    gl_Position = camera.proj * camera.view * vec4(fragPos, 1.0);
}
