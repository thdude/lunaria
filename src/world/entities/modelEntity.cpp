#include "modelEntity.h"

namespace lunaria
{
    void ModelEntity::Start()
    {
        components.push_back(ModelComponent());
        static_cast<ModelComponent>(components[0]).SetModel("monkey.glb");
    }
}