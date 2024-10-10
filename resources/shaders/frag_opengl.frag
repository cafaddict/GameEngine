#version 410

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

// Support up to 8 textures
uniform sampler2D textures[8];
uniform int activeTextureIndex;

void main() {
    // Sample the texture based on the activeTextureIndex
    vec4 texColor = texture(textures[activeTextureIndex], fragTexCoord);

    // Combine texture color with vertex color
    outColor = texColor * fragColor;
    // outColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
}
