#include "world.h"
#include <chrono>

namespace lunaria
{
    World::World()
    {
        renderingSystem.components = &components;
        renderingSystem.entities = &entities;
    }

    void World::Update()
    {
        
    }

    void World::Tick(float delta)
    {

    }
}