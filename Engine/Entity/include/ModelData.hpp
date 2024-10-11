#pragma once
#include "AssetData.hpp"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <vector>
#include <string>

namespace Engine {

struct BoneInfo {
    std::string name;
    int id;                 // Unique identifier for the bone
    glm::mat4 offsetMatrix; // The offset matrix transforms from mesh space to bone space

    BoneInfo() : id(-1), offsetMatrix(1.0f) {}
};

struct KeyFrame {
    double timeStamp;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    // Default constructor
    KeyFrame() : timeStamp(0.0), position(0.0f), rotation(1.0, 0.0, 0.0, 0.0), scale(1.0f) {}

    // Constructor for initializing position keyframes
    KeyFrame(double time, const glm::vec3 &pos)
        : timeStamp(time), position(pos), rotation(1.0, 0.0, 0.0, 0.0), scale(1.0f) {}

    // Constructor for initializing rotation keyframes
    KeyFrame(double time, const glm::quat &rot) : timeStamp(time), position(0.0f), rotation(rot), scale(1.0f) {}

    // Constructor for initializing scale keyframes
    KeyFrame(double time, const glm::vec3 &scl, bool isScale)
        : timeStamp(time), position(0.0f), rotation(1.0, 0.0, 0.0, 0.0), scale(scl) {}
};

struct AnimationChannel {
    std::string boneName;
    std::vector<KeyFrame> positionKeys;
    std::vector<KeyFrame> rotationKeys;
    std::vector<KeyFrame> scalingKeys;
};

struct Animation {
    std::string name;
    double duration; // Duration of the animation in ticks
    double ticksPerSecond;
    std::vector<AnimationChannel> channels;

    Animation() : duration(0.0), ticksPerSecond(25.0) {} // Default to 25 FPS if unspecified
};

class ModelData : public AssetData {
    private:
    void ProcessNode(aiNode *node, const aiScene *scene);
    void ProcessMesh(aiMesh *mesh);
    void ProcessBones(aiMesh *mesh);
    int GetBoneID(const std::string &boneName);
    void AddVertexBoneData(int vertexID, int boneID, float weight);
    void ProcessAnimations(const aiScene *scene);
    void ProcessTextures(const aiScene *scene);

    std::unordered_map<std::string, int> boneMap;
    int boneCount;

    public:
    ModelData();
    ~ModelData();
    bool Load(const std::string &path) override;

    // Mesh data
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> tangents;
    std::vector<uint32_t> indices;

    // Animation data
    std::vector<BoneInfo> bones;
    std::vector<Animation> animations;
    std::vector<std::vector<std::pair<int, float>>> vertexBoneData;

    // Texture data
    std::vector<std::string> texturePaths;
};

} // namespace Engine
