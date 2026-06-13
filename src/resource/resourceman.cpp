#include "resource/resourceman.h"
#include "resource/modelasset.h"
#include <memory>
#include <filesystem>

namespace lunaria
{
    ResourceMan::ResourceMan()
    {
        
    }

    void ResourceMan::FreeAsset(std::string path)
    {
        Asset* asset = assets[path];
        if(asset != nullptr)
        {
            asset->references--;
            if(asset->references < 1)
            {
                assets.erase(path);
                delete asset;
            }
        }
    }
}