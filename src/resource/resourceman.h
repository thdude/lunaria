#pragma once
#include "asset.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <filesystem>

namespace lunaria
{
    template <typename T>
    concept IsDerivedFromBase = std::is_base_of_v<Asset, T>;

    class ResourceMan
    {
        public:
            ResourceMan();

            void FreeAsset(std::string path);

            //test
            std::unordered_map<std::string, Asset*> assets;

            template <typename T>
            requires IsDerivedFromBase<T>
            T* GetAsset(const std::string& path)
            {
                T* asset = static_cast<T *>(assets[path]);
                if(asset == nullptr)
                {
                    assets[path] = new T();
                    asset = static_cast<T*>(assets[path]); //bruh
                    asset->LoadFromFile(std::filesystem::path(path));
                }
                ++asset->references;
                return asset;
            }
        private:
            int butt;
    };
}