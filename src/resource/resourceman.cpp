#include "resource/resourceman.h"
#include "resource/modelasset.h"
#include <memory>
#include <filesystem>

namespace lunaria
{
    ResourceMan::ResourceMan()
    {
        
    }

    template <IsDerivedFromBase T>
    T* ResourceMan::GetAsset(std::string path)
    {
        T* asset = &assets[path];
        if(asset == nullptr)
        {
            assets[path] = std::make_unique<T>();
            T* asset = &assets[path]; //bruh
            asset.LoadFromFile(std::filesystem::path(path));
        }
        return asset;
    }
}