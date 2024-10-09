#version 410

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec2 fragUV;

layout(location = 0) out vec4 outColor;
uniform sampler2D texSampler; // Removed Vulkan-style set and binding

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    outColor = texColor * fragColor; // Combine texture color with vertex color
    // outColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
}