#version 410 core

in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragTangent;
in vec3 fragBitangent;
in vec3 fragWorldPos;

out vec4 fragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D aoMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos;

vec3 calculateNormal() {
    vec3 normal = texture(normalMap, fragUV).rgb;
    normal = normal * 2.0 - 1.0; // Convert from [0,1] range to [-1,1]

    // Convert normal to world space
    vec3 T = normalize(fragTangent);
    vec3 B = normalize(fragBitangent);
    vec3 N = normalize(fragNormal);

    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * normal);
}

void main() {
    vec3 albedo = texture(albedoMap, fragUV).rgb;
    vec3 normal = calculateNormal();
    float ao = texture(aoMap, fragUV).r;

    // Hardcoded PBR parameters for simplicity
    float metallic = 0.0; // Assume non-metallic surface
    float roughness = 0.5;

    // Ambient light
    vec3 ambient = vec3(0.03) * albedo * ao;

    // Light direction and view direction
    vec3 lightDir = normalize(lightPos - fragWorldPos);
    vec3 viewDir = normalize(cameraPos - fragWorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    // Diffuse (Lambertian) reflection
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedo * lightColor;

    // Specular reflection (simplified Cook-Torrance)
    float NDF = pow(max(dot(normal, halfDir), 0.0), roughness);
    float kS = NDF; // Simplified specular term
    vec3 specular = kS * lightColor;

    // Combine ambient, diffuse, and specular
    vec3 finalColor = ambient + diffuse + specular;
    fragColor = vec4(finalColor, 1.0);
}
