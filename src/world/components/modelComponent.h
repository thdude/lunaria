#pragma once
#include "world/component.h"
#include "resource/modelasset.h"
#include "utils/math.h"
#include <string>

namespace lunaria
{
    class ModelComponent : public Component
    {
        public:
            math::Transform3D transform;
            ModelAsset* model;

            void SetModel(std::string path);
    };
}