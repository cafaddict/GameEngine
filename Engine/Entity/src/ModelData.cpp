#include "ModelData.hpp"
#include "glm/fwd.hpp"
#include <cstddef>
#include <iterator>
#include <stb_image/stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>
#include <Log.hpp>
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

namespace Engine {

ModelData::ModelData() {}
ModelData::~ModelData() {}

bool ModelData::Load(const std::string &modelPath) {

  // Create an instance of the Importer class
    Assimp::Importer importer;

    // Load the model
    const aiScene* scene = importer.ReadFile(modelPath, 
        aiProcess_Triangulate | 
        aiProcess_GenNormals |
        aiProcess_FlipUVs);

    // Check if the model was loaded successfully
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        ENGINE_ERROR("Error loading model: {0}", importer.GetErrorString());
        return false;
    }

    // Process the model
    // Assuming you want to process the first mesh in the model
    size_t assimp_index_cnt = 0;
    if (scene->mNumMeshes > 0) {
        aiMesh* mesh = scene->mMeshes[0];

        // Clear previous data
        positions.clear();
        normals.clear();
        uvs.clear();

        // Process vertices
        positions.reserve(mesh->mNumVertices);
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D& pos = mesh->mVertices[i];
            glm::vec3 vertexPosition = {pos.x, pos.y, pos.z};
            positions.push_back(vertexPosition);
            indices.push_back(assimp_index_cnt);
            assimp_index_cnt++;
        }

        // Process normals
        normals.reserve(mesh->mNumVertices);
        if (mesh->HasNormals()) {
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                const aiVector3D& norm = mesh->mNormals[i];
                glm::vec3 vertexNormal = {norm.x, norm.y, norm.z};
                normals.push_back(vertexNormal);
            }
        } else {
            ENGINE_WARN("Mesh does not contain normals.");
        }

        // Process UV coordinates
        uvs.reserve(mesh->mNumVertices);
        if (mesh->HasTextureCoords(0)) { // Assuming texture coordinates are in channel 0
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
                const aiVector3D& uv = mesh->mTextureCoords[0][i];
                glm::vec2 vertexUV = {uv.x, uv.y};
                uvs.push_back(vertexUV);
            }
        } else {
            ENGINE_WARN("Mesh does not contain UV coordinates.");
        }
    } else {
        ENGINE_ERROR( "No meshes found in the model.");
        return false;
    }

    return true;
}
} // namespace Engine
