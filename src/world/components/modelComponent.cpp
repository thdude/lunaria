#include "modelComponent.h"
#include "engine.h"
#include "resource/resourceman.h"

namespace lunaria
{
    void ModelComponent::SetModel(std::string path)
    {
        if(model != nullptr)
        {
            Engine::resourceman.FreeAsset(path);
        }
        model = Engine::resourceman.GetAsset(path);
    }
}