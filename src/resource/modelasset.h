#pragma once
#include "resource/asset.h"
#include "fastgltf/core.hpp"
#include <string>
#include <filesystem>

namespace lunaria
{
    class ModelAsset : public Asset
    {
        public:
            bool LoadFromFile(std::filesystem::path path);
    };
}