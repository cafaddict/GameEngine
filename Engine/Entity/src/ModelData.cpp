#include "ModelData.hpp"
#include <stb_image/stb_image.h>
#include <tinyobjloader/tiny_obj_loader.h>
namespace Engine {

    ModelData::ModelData() {

        }
    ModelData::~ModelData() {

        }

    bool ModelData::Load(const std::string& modelPath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
            modelPath.c_str()))
            {
            throw std::runtime_error(warn + err);
            return false;
            }
        uint32_t indexcount = 0;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                glm::vec3 position = { attrib.vertices[3 * index.vertex_index + 0],
                          attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2] };
                glm::vec3 normal = { attrib.normals[3 * index.normal_index + 0],
                                     attrib.normals[3 * index.normal_index + 1],
                                     attrib.normals[3 * index.normal_index + 2] };
                glm::vec2 uv = { attrib.texcoords[2 * index.texcoord_index + 0],
                                 attrib.texcoords[2 * index.texcoord_index + 1] };
                positions.push_back(position);
                normals.push_back(normal);
                uvs.push_back(uv);
                indices.push_back(indexcount);
                indexcount++;

                }
            }
        return true;
        }

    }