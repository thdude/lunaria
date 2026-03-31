#include "component.h"
#include <vector>

namespace lunaria
{
    class Entity
    {
        public:
            std::vector<Component> components;

            void Start();
            void Tick(float delta);
    };
}