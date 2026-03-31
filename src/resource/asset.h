#pragma once
#include <string>

namespace lunaria
{
    class Asset
    {
        public:
            std::string path;

            virtual void LoadFromFile();
    };
};