#include "resource/resourceman.h"
#include "resource/modelasset.h"
#include <memory>

namespace lunaria
{
    ResourceMan::ResourceMan()
    {
        
    }
    // ModelAsset* ResourceMan::GetModel(std::string path)
    // {
    //     ModelAsset* model = &models[path];
    //     if(model == nullptr)
    //     {
    //         model = new ModelAsset;
    //         model->LoadFromFile(path);
    //     }
    // }

    template <IsDerivedFromBase T>
    T* ResourceMan::GetAsset(std::string path)
    {
        T* asset = &assets[path];
        if(asset == nullptr)
        {
            assets[path] = std::make_unique<T>();
            T* asset = &assets[path]; //bruh
        }
        return asset;
    }
}