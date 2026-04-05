#pragma once
#include "resource/asset.h"
#include "fastgltf/core.hpp"
#include <string>
#include <filesystem>

namespace lunaria
{
    class ModelAsset : Asset
    {
        bool LoadFromFile(std::filesystem::path path);
    };
}