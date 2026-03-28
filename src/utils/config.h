#pragma once

#include "commongl.h"

constexpr auto s_max_bone_influence = 4;

// core
enum class GraphicsAPI {
    OPEN_GL,
    VULKAN
};

enum class CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class CameraMode {
    STANDING,
    CROUCHING
};

// renderer
enum class RenderMode {
    STANDARD = GL_FILL,
    WIREFRAME = GL_LINE,
    POINTCLOUD = GL_POINT
};

// scene
enum class SceneID {
    SOLDIER,
    LIGHTS_ROOM,
    FPS_GAME
};

// graphics
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int BoneIDs[s_max_bone_influence];
    float Weights[s_max_bone_influence];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};