#pragma once
#include <vector>

namespace lunaria
{
    class System
    {
        public:
            std::vector<Entity*>* entities;
            std::vector<Component*>* components;
    };
}