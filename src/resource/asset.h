#pragma once
#include <string>

namespace lunaria
{
    class Asset
    {
        public:
            std::string name;
            uint32_t references = 0;

            void LoadFromFile();
    };
};