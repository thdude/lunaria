#include "entity.h"
#include "component.h"
#include "system.h"
#include <vector>

#include "systems/renderingSystem.h"

namespace lunaria
{
    template <typename T>
    concept IsDerivedFromBase = std::is_base_of<Entity, T>::value;

    class World
    {
        public:
            std::vector<Entity*> entities;
            std::vector<Component*> components;
            bool running = true;
            float time = 0;

            World();
            void Update();
            void Tick(float delta);

            RenderingSystem renderingSystem;

            template <IsDerivedFromBase T>
            T* SpawnEntity()
            {
                Entity* entity = new T();
                entities.push_back(entity);
                entity->Start();
            }
    };
}