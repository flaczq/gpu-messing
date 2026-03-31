#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../cores/camera.h"
#include "../graphics/graphics_types.hpp"
#include "../graphics/mesh.h"
#include "../graphics/model.h"
#include "../managers/resource_manager.h"
#include "../game/entities/game_object.h"
#include "scene.h"
#include "soldier_scene.h"
#include <memory>
#include <vector>

SoldierScene::SoldierScene(Camera* camera)
    : m_camera(camera)
{
}

void SoldierScene::init() {
    m_soldier = std::make_unique<Model>("../assets/models/Soldier.glb");

    std::vector<Vertex> lightVertices = calculateLightVertices();
    std::vector<unsigned int> lightIndices = calculateLightIndices();
    std::vector<Texture*> lightTextures;
    m_lightMarker = std::make_unique<Mesh>(lightVertices, lightIndices, lightTextures);

    auto soldierGO = std::make_shared<GameObject>();
    soldierGO->addComponent
}

void SoldierScene::fixedUpdate(float fixedt) {
    // SOLDIER
    static float time = 0.0f;
    time += fixedt;
    float x = sin(time);
    float z = cos(time);
    m_lightDir = glm::normalize(glm::vec3(x, -1.0f, z));

    m_rotation += m_rotationSpeed * fixedt;
    if (m_rotation >= 360.0f) {
        m_rotation -= 360.0f;
    }

    // transformation matrix: clip = projectionM * viewM * modelM * local
    // 1. local * modelM            -> world
    // 2. world * viewM             -> space (lookAt())
    // 3. space * projectionM       -> clip
    // 4. clip  * viewportTransform -> screen
    if (m_soldierModels.capacity() < soldiersCount) {
        m_soldierModels.reserve(soldiersCount);
    }
    m_soldierModels.clear();
    int side = static_cast<int>(std::sqrt(soldiersCount));
    float spacing = 1.0f;
    for (size_t i{}; i < soldiersCount; i++) {
        float gridX = static_cast<float>(i % side);
        float gridZ = static_cast<float>(i / side);
        glm::vec3 relativePos = glm::vec3(gridX * spacing, 0.0f, gridZ * spacing);
        glm::vec3 tr = relativePos + m_soldierPos;
        float angle = i * 30.0f;

        glm::mat4 soldierModel = glm::mat4(1.0f);
        soldierModel = glm::translate(soldierModel, tr);
        soldierModel = glm::rotate(soldierModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //soldierModel = glm::rotate(soldierModel, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        soldierModel = glm::scale(soldierModel, glm::vec3(100.0f));
        m_soldierModels.push_back(soldierModel);
    }

    // projection
    m_projection = glm::mat4(1.0f);
    m_projection = glm::perspective(glm::radians(m_camera->getFov()), m_camera->getAspect(), 0.1f, 100.0f);

    // view
    m_view = glm::mat4(1.0f);
    m_view = m_camera->getViewMatrix();

    // additional
    m_normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_soldierModels[0])));

    // LIGHT
    glm::vec3 lightMarkerPos = m_soldierPos - (glm::normalize(m_lightDir) * 5.0f);
    lightMarkerPos.y += 2.0f;
    m_lightModel = glm::mat4(1.0f);
    m_lightModel = glm::translate(m_lightModel, lightMarkerPos);
    m_lightModel = glm::scale(m_lightModel, glm::vec3(0.2f));
}

void SoldierScene::renderFrame(float alpha) {
    for (auto& gameObject : m_gameObjects) {
        gameObject->draw();
    }

    // SOLDIERS
    /*auto* soldierShader = ResourceManager::getInstance().getShader("model_shader");
    soldierShader->use();

    soldierShader->setMat4fv("projection", m_projection);
    soldierShader->setMat4fv("view", m_view);
    soldierShader->setMat3fv("normalMatrix", m_normalMatrix);

    soldierShader->setVec3fv("lightDir", m_lightDir);
    soldierShader->setVec3fv("lightColor", glm::vec3(1.0f));
    soldierShader->setVec3fv("viewPos", m_camera->getPosition());

    for (const auto& soldierModel : m_soldierModels) {
        soldierShader->setMat4fv("model", soldierModel);
        m_soldier->draw(*soldierShader);
    }

    // LIGHT
    auto* lightShader = ResourceManager::getInstance().getShader("light_shader");
    lightShader->use();

    lightShader->setMat4fv("projection", m_projection);
    lightShader->setMat4fv("view", m_view);
    lightShader->setMat4fv("model", m_lightModel);

    m_lightMarker->draw(*lightShader);*/
}

void SoldierScene::end() {
    //audio.stop()
}

std::vector<Vertex> SoldierScene::calculateLightVertices() {
    std::vector<Vertex> vertices;
    vertices.reserve(24);

    struct RawData { glm::vec3 pos; glm::vec3 norm; };

    RawData raw[] = {
        // back (normal: 0, 0, -1)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}}, {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
        // front (normal: 0, 0, 1)
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}}, {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}}, {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f,  1.0f}},
        // left (normal: -1, 0, 0)
        {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}}, {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}},
        // right (normal: 1, 0, 0)
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}}, {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
        // down (normal: 0, -1, 0)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}}, {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}}, {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}},
        // up (normal: 0, 1, 0)
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}
    };

    for (int i = 0; i < 24; ++i) {
        Vertex vertex{};
        vertex.Position = raw[i].pos;
        vertex.Normal = raw[i].norm;

        vertices.push_back(vertex);
    }

    return vertices;
}

std::vector<unsigned int> SoldierScene::calculateLightIndices() {
    return {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
}