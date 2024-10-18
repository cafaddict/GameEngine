#version 450

// Inputs from the vertex shader
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec4 fragColor;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec3 fragTangent;
layout(location = 5) in vec3 fragBitangent;

// Outputs to the framebuffer
layout(location = 0) out vec4 outColor;

// UBO for camera matrices (already defined in the vertex shader)
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

// Samplers for material textures
layout(set = 0, binding = 3) uniform sampler2D albedoMap;
layout(set = 0, binding = 4) uniform sampler2D normalMap;
layout(set = 0, binding = 5) uniform sampler2D aoMap;

// Calculate normal from the normal map, transformed into world space
vec3 calculateNormal() {
    vec3 normal = texture(normalMap, fragTexCoord).rgb;
    normal = normal * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]

    // Transform the normal from tangent space to world space using the TBN matrix
    vec3 T = normalize(fragTangent);
    vec3 B = normalize(fragBitangent);
    vec3 N = normalize(fragNormal);
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * normal);
}

void main() {
    // Sample textures
    vec3 albedo = texture(albedoMap, fragTexCoord).rgb;
    float ao = texture(aoMap, fragTexCoord).r;

    // Compute normal from the normal map
    vec3 normal = calculateNormal();

    // Calculate view direction and light direction
    vec3 viewDir = normalize(camera.cameraPos - fragPos);
    vec3 lightDir = normalize(light.lightPos - fragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    // Lambertian diffuse reflection
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedo * light.lightColor * light.lightIntensity;

    // Specular reflection using simplified Cook-Torrance model
    float roughness = 0.5; // Roughness is hardcoded for simplicity
    float NDF = pow(max(dot(normal, halfDir), 0.0), roughness);
    float kS = NDF; // Simplified specular reflectance term
    vec3 specular = kS * light.lightColor * light.lightIntensity;

    // Ambient lighting, modulated by the AO map
    vec3 ambient = albedo * ao;

    // Final color combining ambient, diffuse, and specular lighting
    vec3 finalColor = ambient + diffuse + specular;

    // Output the final color, modulated by vertex color
    outColor = vec4(finalColor * fragColor.rgb, fragColor.a);
    // outColor = vec4(texture(normalMap, fragTexCoord), 1.0);
    outColor = vec4(ambient, 1.0);
    
}
