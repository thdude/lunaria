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
#include <vector>

template <>
struct fastgltf::ElementTraits<glm::vec3> : fastgltf::ElementTraitsBase<glm::vec3, AccessorType::Vec3, float> {};

template <>
struct fastgltf::ElementTraits<glm::vec2> : fastgltf::ElementTraitsBase<glm::vec2, AccessorType::Vec2, float> {};

namespace lunaria
{
    bool ModelAsset::LoadFromFile(std::filesystem::path path)
    {
        fastgltf::Expected<fastgltf::GltfDataBuffer> dataFile = fastgltf::GltfDataBuffer::FromPath(path);
        fastgltf::GltfDataBuffer data;
        if (dataFile)
        {
            data = std::move(dataFile.get());
        }
        else
        {
            return false;
        }

        constexpr auto gltfOptions = fastgltf::Options::LoadExternalBuffers;

        fastgltf::Parser parser{};

        std::cout << path.relative_path() << std::endl;

        auto asset = parser.loadGltfBinary(data, std::filesystem::current_path(), gltfOptions);
        if (asset.error() != fastgltf::Error::None) {
            std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(asset.error()) << '\n';
            return false;
        }

        std::cout << "loading model...." << std::endl;

        fastgltf::Asset gltf = std::move(asset.get());

        fastgltf::Mesh& mesh = gltf.meshes[0];

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
            //fastgltf::Accessor& uvAccessor = gltf.accessors[p.findAttribute("TEXCOORD_0")->accessorIndex];
            vertices.reserve(vertices.size() + posAccessor.count);
            fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor, [&](glm::vec3 pos, uint32_t index)
            {
                glm::vec3 norm = fastgltf::getAccessorElement<glm::vec3>(gltf, normAccessor, index);
                //glm::vec2 uv = fastgltf::getAccessorElement<glm::vec2>(gltf, uvAccessor, index);

                Vertex vert;
                vert.position = {-pos.z, pos.x, pos.y};
                vert.normal = {-norm.z, norm.x, norm.y};
                // vert.uvX = uv.x;
                // vert.uvY = uv.y;

                vertices.push_back(vert);
            });
        }

        std::cout << "hello" << std::endl;
        Engine::renderer->UploadMesh(vertices.data(), vertices.size(), indices.data(), indices.size());

        return true;
    }
}