#include "ModelData.hpp"
#include "assimp/anim.h"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <Log.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstddef>
#include <iterator>
#include <stb_image/stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>

namespace Engine {

ModelData::ModelData() : boneCount(0) {}
ModelData::~ModelData() {}

bool ModelData::Load(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ENGINE_ERROR("Error loading model: {0}", importer.GetErrorString());
        return false;
    }

    // Clear previous data
    meshes.clear();
    bones.clear();
    animations.clear();
    texturePaths.clear();
    boneMap.clear();
    boneCount = 0;

    // Process nodes recursively to load all meshes
    ProcessNode(scene->mRootNode, scene);

    // Process animations and textures if available
    if (scene->HasAnimations()) {
        ProcessAnimations(scene);
    }

    ProcessTextures(scene);

    return true;
}

void ModelData::ProcessNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(ProcessMesh(mesh));

        meshes.push_back(newMesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh ModelData::ProcessMesh(aiMesh *mesh) {
    Mesh meshData;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        meshData.positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));

        if (mesh->HasNormals()) {
            meshData.normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        }

        if (mesh->HasTextureCoords(0)) {
            meshData.uvs.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        }

        if (mesh->HasTangentsAndBitangents()) {
            meshData.tangents.push_back(glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
        }
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            meshData.indices.push_back(face.mIndices[j]);
        }
    }

    // Process bones if available
    if (mesh->HasBones()) {
        ProcessBones(mesh, meshData);
    }

    return meshData;
}

void ModelData::ProcessBones(aiMesh *mesh, Mesh &meshData) {
    for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
        aiBone *bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();
        int boneID = GetBoneID(boneName);
        bones[boneID].offsetMatrix = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));

        for (unsigned int j = 0; j < bone->mNumWeights; ++j) {
            const aiVertexWeight &weight = bone->mWeights[j];
            AddVertexBoneData(weight.mVertexId, boneID, weight.mWeight, meshData);
        }
    }
}

int ModelData::GetBoneID(const std::string &boneName) {
    if (boneMap.find(boneName) == boneMap.end()) {
        int id = boneCount++;
        boneMap[boneName] = id;
        bones.push_back(BoneInfo());
        bones[id].name = boneName;
    }
    return boneMap[boneName];
}

void ModelData::AddVertexBoneData(int vertexID, int boneID, float weight, Mesh &meshData) {
    if (meshData.vertexBoneData.size() <= vertexID) {
        meshData.vertexBoneData.resize(vertexID + 1);
    }
    meshData.vertexBoneData[vertexID].push_back(std::make_pair(boneID, weight));
}

void ModelData::ProcessAnimations(const aiScene *scene) {
    // Same as in your original code
}

void ModelData::ProcessTextures(const aiScene *scene) {
    // Same as in your original code
}

} // namespace Engine
