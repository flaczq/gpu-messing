#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/game_entity.h"
#include "../graphics/graphics_types.hpp"
#include "../graphics/mesh.h"
#include "../graphics/model.h"
#include "../managers/resource_manager.h"
#include "scene.h"
#include "soldier_scene.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

SoldierScene::SoldierScene(Camera* camera)
    : m_camera(camera)
{
}

void SoldierScene::init() {
    auto& resourceManager = ResourceManager::getInstance();
    resourceManager.loadModel("soldier_model", "../assets/models/Soldier.glb");
    resourceManager.loadShader("model_shader", "../shaders/model.vert", "../shaders/model.frag");
    resourceManager.loadShader("light_shader", "../shaders/light.vert", "../shaders/light.frag");
    resourceManager.loadShader("floor_shader", "../shaders/grid.vert", "../shaders/grid.frag");

    // FLOOR
    auto floorGO = std::make_unique<GameEntity>("floor");
    floorGO->addComponent<TransformComponent>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    floorGO->addComponent<RenderComponent>(nullptr, nullptr);
    //floorGO->setAlive(false);
    floorGO->init();
    m_gameEntities.push_back(std::move(floorGO));

    // SOLDIER
    auto soldierModel = resourceManager.getModel("soldier_model");
    if (!soldierModel) {
        LOG_E("No model for soldier...");
        return;
    }
    auto modelShader = resourceManager.getShader("model_shader");
    if (!modelShader) {
        LOG_E("No shader for soldier...");
        return;
    }

    float spacing = 1.5f;
    for (size_t i{}; i < 100; i++) {
        unsigned int row = i / 10;
        unsigned int col = i % 10;
        glm::vec3 pos = SOLDIER_POSITION + glm::vec3(col * spacing, 0.0f, row * spacing);
        glm::quat rotQ = glm::angleAxis(SOLDIER_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
        auto soldierGO = std::make_unique<GameEntity>("soldier_" + std::to_string(i));
        soldierGO->addComponent<TransformComponent>(pos, rotQ, SOLDIER_SCALE);
        soldierGO->addComponent<RenderComponent>(soldierModel, modelShader);
        soldierGO->init();
        m_gameEntities.push_back(std::move(soldierGO));
    }

    // LIGHT
    std::vector<Vertex> lightVertices = calculateLightVertices();
    std::vector<unsigned int> lightIndices = calculateLightIndices();
    std::vector<std::shared_ptr<Texture>> lightTextures;
    m_lightMarker = std::make_unique<Mesh>(lightVertices, lightIndices, lightTextures);
}

void SoldierScene::saveState() {
    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }

        gameEntity->getTransform()->saveState();
    }
}

void SoldierScene::fixedUpdate(float fixedt) {
    // transformation matrix: clip = projectionM * viewM * modelM * local
    // 1. local * modelM            -> world
    // 2. world * viewM             -> space (lookAt())
    // 3. space * projectionM       -> clip
    // 4. clip  * viewportTransform -> screen
    // SOLDIER
    /*static float time = 0.0f;
    time += fixedt;
    float x = sin(time);
    float z = cos(time);
    m_lightDir = glm::normalize(glm::vec3(x, -1.0f, z));*/

    float time = static_cast<float>(glfwGetTime());
    glm::quat newRot = glm::angleAxis(SOLDIER_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(90.0f) * time, glm::vec3(0.0f, 0.0f, 1.0f));
    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }

        gameEntity->fixedUpdate(fixedt);

        if (gameEntity->getName().starts_with("soldier_")) {
            auto nr = gameEntity->getName().substr(gameEntity->getName().find_last_of('_') + 1);
            if (std::stoi(nr) % 3 == 0) {
                gameEntity->getTransform()->setRotation(newRot);
            }
        }
    }
}

void SoldierScene::update(float alpha) {
    //    ┳┓┏┓┳┓┳┓┏┓┳┓  ┏┓┳┓┏┓┳┳┓┏┓
    //    ┣┫┣ ┃┃┃┃┣ ┣┫  ┣ ┣┫┣┫┃┃┃┣ 
    //    ┛┗┗┛┛┗┻┛┗┛┛┗  ┻ ┛┗┛┗┛ ┗┗┛
    //                             
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(m_camera->getFov()), m_camera->getAspect(), m_camera->getNearPlane(), m_camera->getFarPlane());
    glm::mat4 view = m_camera->getViewMatrix();
    glm::vec3 viewPos = m_camera->getPosition();

    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }

        gameEntity->update(alpha);

        auto* shader = gameEntity->getRender()->getShader();
        shader->use();

        shader->setMat4fv("projection", projection);
        shader->setMat4fv("view", view);

        shader->setVec3fv("lightDir", m_lightDir);
        shader->setVec3fv("lightColor", glm::vec3(1.0f));
        shader->setVec3fv("viewPos", viewPos);

        gameEntity->getRender()->draw(alpha);
    }

    // LIGHT
    glm::vec3 lightMarkerPos = SOLDIER_POSITION - (glm::normalize(m_lightDir) * 5.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightMarkerPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));

    auto lightShader = ResourceManager::getInstance().getShader("light_shader");
    lightShader->use();

    lightShader->setMat4fv("projection", projection);
    lightShader->setMat4fv("view", view);
    lightShader->setMat4fv("model", lightModel);

    m_lightMarker->draw(*lightShader);
}

void SoldierScene::end() {
    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }

        gameEntity->end();
    }
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