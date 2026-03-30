#pragma once
#include <glm/glm.hpp>

namespace lunaria::math
{
    class Transform3D
    {
        public:
            glm::mat4 GetTransformMat();
            glm::vec3 GetForwardVector();
            glm::vec3 GetRightVector();
            glm::vec3 GetUpVector();
            glm::mat4 GetViewMatrix();
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale = glm::vec3(1);
    };    
}
