#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "utils/transform3d.h"

namespace lunaria::math
{
    glm::mat4 Transform3D::GetTransformMat()
    {
        glm::quat aroundX = glm::angleAxis(glm::radians(this->rotation.x), glm::vec3(1.0, 0.0, 0.0));
        glm::quat aroundY = glm::angleAxis(glm::radians(this->rotation.y), glm::vec3(0.0, 1.0, 0.0));
        glm::quat aroundZ = glm::angleAxis(glm::radians(this->rotation.z), glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 rotationMat = glm::mat4_cast(aroundZ * aroundY * aroundX);

        return glm::scale(glm::translate(glm::mat4(1.0f), this->position) * rotationMat, this->scale);
    }

    glm::mat4 MakeViewMatrix(glm::vec3 forward, glm::vec3 right, glm::vec3 up, glm::vec3 position)
    {
        glm::mat4 view = {};
        view[0] = {right.x, up.x, forward.x, 0};
        view[1] = {right.y, up.y, forward.y, 0};
        view[2] = {right.z, up.z, forward.z, 0};
        view[3] = {-glm::dot(right, position),
                -glm::dot(up, position),
                -glm::dot(forward, position),
                1};

        return view;
    }

    glm::vec3 Transform3D::GetForwardVector()
    {
        return GetTransformMat() * glm::vec4(1.0, 0.0, 0.0, 0.0);
    }

    glm::vec3 Transform3D::GetRightVector()
    {
        return GetTransformMat() * glm::vec4(0.0, 1.0, 0.0, 0.0);
    }

    glm::vec3 Transform3D::GetUpVector()
    {
        return GetTransformMat() * glm::vec4(0.0, 0.0, 1.0, 0.0);
    }

    glm::mat4 Transform3D::GetViewMatrix()
    {
        glm::vec3 forward = GetForwardVector();
        glm::vec3 right = GetRightVector();
        glm::vec3 up = GetUpVector();

        return MakeViewMatrix(forward, right, up, GetTransformMat() * glm::vec4(0, 0, 0, 1));
    }
}