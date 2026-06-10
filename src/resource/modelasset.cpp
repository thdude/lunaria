#include "modelasset.h"
#include "engine.h"
#include "rendering/renderer.h"
#include "fastgltf/core.hpp"
#include "fastgltf/types.hpp"
#include "fastgltf/tools.hpp"
#include "glm/glm.hpp"
#include <string>
#include <filesystem>
#include <iostream>

template <>
struct fastgltf::ElementTraits<glm::vec3> : fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};

template <>
struct fastgltf::ElementTraits<glm::vec2> : fastgltf::ElementTraitsBase<glm::vec2, AccessorType::Vec2, float> {};

namespace lunaria
{
    bool ModelAsset::LoadFromFile(std::filesystem::path path)
    {
        {
            static constexpr auto supportedExtensions =
                fastgltf::Extensions::KHR_mesh_quantization |
                fastgltf::Extensions::KHR_texture_transform |
                fastgltf::Extensions::KHR_materials_variants;

            fastgltf::Parser parser(supportedExtensions);

            constexpr auto gltfOptions =
                fastgltf::Options::DontRequireValidAssetMember |
                fastgltf::Options::AllowDouble |
                fastgltf::Options::LoadExternalBuffers |
                fastgltf::Options::LoadExternalImages |
                fastgltf::Options::GenerateMeshIndices;

            auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
            if (!bool(gltfFile)) {
                std::cerr << "Failed to open glTF file: " << fastgltf::getErrorMessage(gltfFile.error()) << '\n';
                return false;
            }

            auto asset = parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);
            if (asset.error() != fastgltf::Error::None) {
                std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(asset.error()) << '\n';
                return false;
            }

            fastgltf::Asset gltf = std::move(asset.get());

            auto mesh = gltf.meshes[0];

            //copied from enator
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            for (fastgltf::Primitive &p : mesh.primitives)
            {
                fastgltf::Accessor& indexAccessor = gltf.accessors[p.indicesAccessor.value()];
                indices.reserve(indices.size() + indexAccessor.count);
                fastgltf::iterateAccessor<uint32_t>(gltf, indexAccessor, [&](uint32_t index)
                {
                    indices.push_back(index);
                });

                fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
                fastgltf::Accessor& normAccessor = gltf.accessors[p.findAttribute("NORMAL")->accessorIndex];
                fastgltf::Accessor& uvAccessor = gltf.accessors[p.findAttribute("TEXCOORD_0")->accessorIndex];
                vertices.reserve(vertices.size() + posAccessor.count);
                fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 pos, uint32_t index)
                {
                    glm::vec3 norm = fastgltf::getAccessorElement<glm::vec3>(gltf, normAccessor, index);
                    glm::vec2 uv = fastgltf::getAccessorElement<glm::vec2>(gltf, uvAccessor, index);

                    Vertex vert;
                    vert.position = {-pos.z, pos.x, pos.y};
                    vert.normal = {-norm.z, norm.x, norm.y};
                    // vert.uvX = uv.x;
                    // vert.uvY = uv.y;

                    vertices.push_back(vert);
                });
            }

            Engine::renderer->UploadMesh(vertices.data(), vertices.size(), indices.data(), indices.size());
        }
    }
}