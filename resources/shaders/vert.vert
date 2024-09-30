#version 450

// UBO for camera matrices
layout(set = 0, binding = 0) uniform CameraUBO {
    mat4 view;
    mat4 proj;
}
camera;

// UBO for lighting parameters
layout(set = 0, binding = 1) uniform LightUBO {
    vec3 lightPos;
    vec3 lightColor;
}
light;

// SSBO for transformation matrices
layout(set = 0, binding = 3) readonly buffer StorageBufferObject { mat4 transformations[]; }
ssbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec2 fragUV;

void main() {
    // uint instanceIndex = gl_InstanceIndex; // Ensure instance index is used correctly
    // mat4 model = ssbo.transformations[instanceIndex];

    // gl_Position = camera.proj * camera.view * model * vec4(inPosition, 1.0);
    // fragColor = inColor;
    // fragTexCoord = inTexCoord;
    // vec3 normal = inNormal;
    // if (length(normal) == 0.0) {
    //     normal = vec3(0.0, 0.0, 1.0); // Default normal
    // }

    // fragNormal = mat3(model) * normal;
    // Define positions for a full-screen triangle
    // These positions are in Normalized Device Coordinates (NDC)
    vec2 positions[3] = vec2[](vec2(-1.0, -1.0), // Bottom-left
                               vec2(3.0, -1.0),  // Bottom-right beyond the screen
                               vec2(-1.0, 3.0)   // Top-left beyond the screen
    );

    fragUV = (positions[gl_VertexIndex] + 1.0) * 0.5; // Convert NDC to UV coordinates

    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}