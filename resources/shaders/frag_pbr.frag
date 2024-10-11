#version 410

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec4 fragColor;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec3 fragTangent;
layout(location = 5) in vec3 fragBitangent;

layout(location = 0) out vec4 outColor;

// Uniforms for PBR materials
uniform sampler2D albedoMap;    // Albedo
uniform sampler2D normalMap;    // Normal map
uniform sampler2D metalicMap;   // Metallic map
uniform sampler2D roughnessMap; // Roughness map
uniform sampler2D aoMap;        // Ambient occlusion map

layout(std140) uniform CameraUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
};

layout(std140) uniform LightUBO {
    vec3 lightPos;
    vec3 lightColor;
    float lightIntensity;
};

const float PI = 3.14159265359;

// Functions to get PBR values
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, fragTexCoord).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(fragPos);
    vec3 Q2 = dFdy(fragPos);
    vec2 st1 = dFdx(fragTexCoord);
    vec2 st2 = dFdy(fragTexCoord);

    vec3 N = normalize(fragNormal);
    vec3 T = normalize(fragTangent);
    vec3 B = cross(N, T);

    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); }

void main() {
    vec3 albedo = pow(texture(albedoMap, fragTexCoord).rgb, vec3(2.2)); // Gamma correction
    float metallic = texture(metalicMap, fragTexCoord).r;
    float roughness = texture(roughnessMap, fragTexCoord).r;
    float ao = texture(aoMap, fragTexCoord).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(cameraPos - fragPos);
    vec3 L = normalize(lightPos - fragPos);
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    vec3 irradiance = lightColor * lightIntensity * NdotL;

    vec3 diffuse = irradiance * albedo / PI;
    vec3 ambient = ao * albedo;

    vec3 color = ambient + (kD * diffuse + specular);
    color = clamp(color, 0.0, 1.0); // Clamp the color values to a valid range

    // Apply tone mapping and gamma correction
    color = color / (color + vec3(1.0)); // Reinhard tone mapping

    // Ensure each component of the color vector is non-negative for gamma correction
    color = pow(max(color, 0.0), vec3(1.0 / 2.2)); // Gamma correction

    outColor = vec4(color, 1.0); // Assign the final color to the output
    // outColor = vec4(albedo, 1.0); // Assign the albedo color to the output

}
