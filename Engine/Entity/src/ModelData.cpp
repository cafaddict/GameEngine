#include "ModelData.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Log.hpp>
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure
#include <cstddef>
#include <iterator>
#include <stb_image/stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>

namespace Engine {

ModelData::ModelData() {}
ModelData::~ModelData() {}

bool ModelData::Load(const std::string &path) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // Load the model
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    // Check if the model was loaded successfully
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ENGINE_ERROR("Error loading model: {0}", importer.GetErrorString());
        return false;
    }

    // Clear previous data
    positions.clear();
    normals.clear();
    uvs.clear();
    tangents.clear();
    indices.clear();
    bones.clear();
    animations.clear();
    vertexBoneData.clear();
    texturePaths.clear();
    boneMap.clear();
    boneCount = 0;

    // Start processing nodes recursively from the root node
    ProcessNode(scene->mRootNode, scene);

    // Process animations if available
    if (scene->HasAnimations()) {
        ProcessAnimations(scene);
    }

    // Process textures in the scene
    ProcessTextures(scene);
    if (!texturePaths.empty()) {
        ENGINE_INFO("Model contains {0} textures.", texturePaths.size());
        for (const auto &texturePath : texturePaths) {
            ENGINE_INFO("Texture path: {0}", texturePath);
        }
    }

    return true;
}

void ModelData::ProcessNode(aiNode *node, const aiScene *scene) {
    // Process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh);
    }

    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void ModelData::ProcessMesh(aiMesh *mesh) {
    size_t startIndex = positions.size(); // To adjust indices per mesh

    // Process vertices
    positions.reserve(startIndex + mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D &pos = mesh->mVertices[i];
        glm::vec3 vertexPosition = {pos.x, pos.y, pos.z};
        positions.push_back(vertexPosition);
    }

    // Process normals
    if (mesh->HasNormals()) {
        normals.reserve(startIndex + mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D &norm = mesh->mNormals[i];
            glm::vec3 vertexNormal = {norm.x, norm.y, norm.z};
            normals.push_back(vertexNormal);
        }
    } else {
        ENGINE_WARN("Mesh does not contain normals.");
    }

    // Process UV coordinates
    if (mesh->HasTextureCoords(0)) { // Assuming texture coordinates are in channel 0
        uvs.reserve(startIndex + mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D &uv = mesh->mTextureCoords[0][i];
            glm::vec2 vertexUV = {uv.x, uv.y};
            uvs.push_back(vertexUV);
        }
    } else {
        ENGINE_WARN("Mesh does not contain UV coordinates.");
    }

    // Process tangents
    if (mesh->HasTangentsAndBitangents()) {
        tangents.reserve(startIndex + mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D &tangent = mesh->mTangents[i];
            glm::vec3 vertexTangent = {tangent.x, tangent.y, tangent.z};
            tangents.push_back(vertexTangent);
        }
    } else {
        ENGINE_WARN("Mesh does not contain tangents.");
    }

    // Process mesh indices (assuming triangles)
    if (mesh->HasFaces()) {
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace &face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j] + startIndex);
            }
        }
    }

    // Process bones if the mesh has bones (for animation support)
    if (mesh->HasBones()) {
        ProcessBones(mesh);
    }
}

void ModelData::ProcessBones(aiMesh *mesh) {
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        aiBone *bone = mesh->mBones[boneIndex];
        std::string boneName = bone->mName.C_Str();

        int boneID = GetBoneID(boneName);
        bones[boneID].offsetMatrix = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));

        // Store bone weights
        for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
            const aiVertexWeight &weight = bone->mWeights[weightIndex];
            int vertexID = weight.mVertexId;
            float weightValue = weight.mWeight;

            AddVertexBoneData(vertexID, boneID, weightValue);
        }
    }
}

int ModelData::GetBoneID(const std::string &boneName) {
    if (boneMap.find(boneName) == boneMap.end()) {
        int newID = boneCount++;
        boneMap[boneName] = newID;
        bones.push_back(BoneInfo());
        bones[newID].name = boneName;
    }
    return boneMap[boneName];
}

void ModelData::AddVertexBoneData(int vertexID, int boneID, float weight) {
    if (vertexBoneData.size() <= vertexID) {
        vertexBoneData.resize(vertexID + 1);
    }
    vertexBoneData[vertexID].push_back(std::make_pair(boneID, weight));
}

void ModelData::ProcessAnimations(const aiScene *scene) {
    for (unsigned int animIndex = 0; animIndex < scene->mNumAnimations; ++animIndex) {
        aiAnimation *animation = scene->mAnimations[animIndex];
        Animation anim;
        anim.name = animation->mName.C_Str();
        anim.duration = animation->mDuration;
        anim.ticksPerSecond = (animation->mTicksPerSecond != 0) ? animation->mTicksPerSecond : 25.0f;

        for (unsigned int channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex) {
            aiNodeAnim *nodeAnim = animation->mChannels[channelIndex];
            AnimationChannel animChannel;
            animChannel.boneName = nodeAnim->mNodeName.C_Str();

            // Process position keyframes
            for (unsigned int keyIndex = 0; keyIndex < nodeAnim->mNumPositionKeys; ++keyIndex) {
                aiVectorKey &key = nodeAnim->mPositionKeys[keyIndex];
                animChannel.positionKeys.emplace_back(key.mTime, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z));
            }

            // Process rotation keyframes
            for (unsigned int keyIndex = 0; keyIndex < nodeAnim->mNumRotationKeys; ++keyIndex) {
                aiQuatKey &key = nodeAnim->mRotationKeys[keyIndex];
                animChannel.rotationKeys.emplace_back(
                    key.mTime, glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z));
            }

            // Process scaling keyframes
            for (unsigned int keyIndex = 0; keyIndex < nodeAnim->mNumScalingKeys; ++keyIndex) {
                aiVectorKey &key = nodeAnim->mScalingKeys[keyIndex];
                animChannel.scalingKeys.emplace_back(key.mTime, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z));
            }

            anim.channels.push_back(animChannel);
        }
        animations.push_back(anim);
    }
}

void ModelData::ProcessTextures(const aiScene *scene) {
    // Iterate through all materials in the scene
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial *material = scene->mMaterials[i];

        // Check for a diffuse texture (you can also check for other types like specular, normal, etc.)
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                texturePaths.push_back(path.C_Str());
            }
        }

        // You can also check for other types like normal maps, specular maps, etc.
        if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
            aiString path;
            if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
                texturePaths.push_back(path.C_Str());
            }
        }

        // Handle other texture types as needed
    }
}

} // namespace Engine
