#version 450


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// UBO for camera matrices
layout(set = 0, binding = 1) uniform CameraUBO {
    mat4 view;
    mat4 proj;
} camera;

// UBO for lighting parameters
layout(set = 0, binding = 2) uniform LightUBO {
    vec3 lightPos;
    vec3 lightColor;
} light;

// // SSBO for transformation matrices
// layout(set = 0, binding = 2) buffer StorageBufferObject {
//     mat4 transformations[];
// } ssbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition,1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}