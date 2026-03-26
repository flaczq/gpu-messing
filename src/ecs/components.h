#pragma once

#include "../utils/commongl.h"
#include "../renderer/model.h"
#include "../renderer/shader.h"

struct TransformComponent {
    glm::vec3 position;
    glm::vec3 rotiation;
    glm::vec3 scale{ 1.0f };
};

struct MeshComponent {
    unsigned int vaoID;
    unsigned int indexCount;
};

struct MaterialComponent {
    unsigned int shaderID;
    unsigned int textureID;
};

struct PointLightComponent {
    glm::vec3 color;
    float intensity;
};

struct CameraComponent {
    float fov;
    float nearPlane;
    float farPlane;
    glm::vec3 up;
    glm::vec3 front;
};