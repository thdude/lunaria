#pragma once
#include "asset.h"
#include "modelasset.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace lunaria
{
    template <typename T>
    concept IsDerivedFromBase = std::is_base_of<Asset, T>::value;


    class ResourceMan
    {
        public:
            ResourceMan();
            std::unordered_map<std::string, ModelAsset> models;
            //std::unordered_map textures;

            //ModelAsset* GetModel(std::string path); 
            void GetTexture();

            //test
            std::unordered_map<std::string, Asset*> assets;

            template <IsDerivedFromBase T>
            T* GetAsset(std::string path)
            {
                T* asset = (T*)assets[path];
                if(asset == nullptr)
                {
                    assets[path] = new T();
                    T* asset = (T*)assets[path]; //bruh
                    asset->LoadFromFile(std::filesystem::path(path));
                }
                return asset;
            }
        private:
            int butt;
    };
}