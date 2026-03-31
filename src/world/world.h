#include "entity.h"
#include "component.h"
#include "system.h"
#include <vector>

namespace lunaria
{
    class World
    {
        public:
            std::vector<Entity> entities;
            bool running = true;
            float time = 0;

            World();
            void Update();
            void Tick(float delta);
    };
}