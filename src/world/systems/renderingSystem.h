#pragma once
#include "world/system.h"
#include "rendering/renderer.h"
#include <vector>

namespace lunaria
{
    class RenderingSystem : public System
    {
        private:
            
        public:
            std::vector<DrawObject> GetRenderObjects();
    };
}