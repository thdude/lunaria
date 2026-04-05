#include "modelasset.h"
#include "fastgltf/core.hpp"
#include <string>
#include <filesystem>
#include <iostream>

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

            viewer->asset = std::move(asset.get());
        }
    }
}