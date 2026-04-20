#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/game_entity.h"
#include "../graphics/graphics_types.hpp"
#include "../graphics/material.h"
#include "../graphics/mesh.h"
#include "../graphics/mesh_generator.h"
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
    // MODELS
    auto floorMesh = MeshGenerator::createCube(16.0f, 3.0f, 15.0f);
    auto floorMeshPtr = std::make_unique<Mesh>(std::move(floorMesh));
    auto floorModel = std::make_shared<Model>(std::move(floorMeshPtr));
    resourceManager.addModel("floor_model", floorModel);
    resourceManager.loadModel("soldier_model", "../assets/models/Soldier.glb");
    // MATERIALS with SHADERS
    resourceManager.loadMaterial("soldier_material", "../shaders/model.vert", "../shaders/model.frag");
    resourceManager.loadMaterial("light_material", "../shaders/light.vert", "../shaders/light.frag");
    resourceManager.loadMaterial("floor_material", "../shaders/lambert.vert", "../shaders/lambert.frag");

    // FLOOR
    floorModel = resourceManager.getModel("floor_model");
    auto floorMaterial = resourceManager.getMaterial("floor_material");
    if (floorModel && floorMaterial) {
        // MATERIAL UNIFORMS
        //floorMaterial->addVec3Uniform("lightColor", glm::vec3(1.0f));

        glm::quat fRotQ = glm::angleAxis(FLOOR_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
        auto floorGO = std::make_unique<GameEntity>("floor", true);
        floorGO->addComponent<TransformComponent>(FLOOR_POSITION, fRotQ, FLOOR_SCALE);
        floorGO->addComponent<RenderComponent>(floorModel, floorMaterial);
        floorGO->init();
        m_gameEntities.push_back(std::move(floorGO));
    }

    // SOLDIER
    auto soldierModel = resourceManager.getModel("soldier_model");
    auto soldierMaterial = resourceManager.getMaterial("soldier_material");
    if (soldierModel && soldierMaterial) {
        // MATERIAL UNIFORMS
        //soldierMaterial->addVec3Uniform("lightColor", glm::vec3(1.0f));

        float spacing = 1.5f;
        for (size_t i{}; i < 100; i++) {
            unsigned int row = i / 10;
            unsigned int col = i % 10;
            glm::vec3 sPos = SOLDIER_POSITION + glm::vec3(col * spacing, 0.0f, row * spacing);
            glm::quat sRotQ = glm::angleAxis(SOLDIER_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
            auto soldierGO = std::make_unique<GameEntity>("soldier_" + std::to_string(i));
            soldierGO->addComponent<TransformComponent>(sPos, sRotQ, SOLDIER_SCALE);
            soldierGO->addComponent<RenderComponent>(soldierModel, soldierMaterial);
            soldierGO->init();
            m_gameEntities.push_back(std::move(soldierGO));
        }
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
        auto* transform = gameEntity->getTransform();
        if (!transform->isActive()) {
            continue;
        }

        transform->saveState();
    }
}

void SoldierScene::fixedUpdate(float fixedt) {
    // transformation matrix: clip = projectionM * viewM * modelM * local
    // 1. local * modelM            -> world
    // 2. world * viewM             -> space (lookAt())
    // 3. space * projectionM       -> clip
    // 4. clip  * viewportTransform -> screen
    // LIGHT
    /*static float tt = 0.0f;
    tt += fixedt;
    float x = sin(tt);
    float z = cos(tt);
    m_lightDir = glm::normalize(glm::vec3(x, -1.0f, z));*/

    // SOLDIER
    float time = static_cast<float>(glfwGetTime());
    glm::quat newRot = glm::angleAxis(SOLDIER_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(90.0f) * time, glm::vec3(0.0f, 0.0f, 1.0f));
    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }
        auto* transform = gameEntity->getTransform();
        if (!transform->isActive()) {
            continue;
        }

        gameEntity->fixedUpdate(fixedt);

        if (gameEntity->getName().starts_with("soldier_")) {
            auto nr = gameEntity->getName().substr(gameEntity->getName().find_last_of('_') + 1);
            if (std::stoi(nr) % 3 == 0) {
                transform->setRotation(newRot);
            }
        }
    }
}

void SoldierScene::update(float alpha) {
    //    ┳┓┏┓┳┓┳┓┏┓┳┓  ┏┓┳┓┏┓┳┳┓┏┓
    //    ┣┫┣ ┃┃┃┃┣ ┣┫  ┣ ┣┫┣┫┃┃┃┣ 
    //    ┛┗┗┛┛┗┻┛┗┛┛┗  ┻ ┛┗┛┗┛ ┗┗┛
    //                             
    glm::mat4 projection = m_camera->getProjection();
    glm::mat4 view = m_camera->getViewMatrix();
    glm::vec3 viewPos = m_camera->getViewPos();
    RenderContext ctx = {
        projection,
        view,
        viewPos
    };

    // TODO shader sorting
    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }
        auto* render = gameEntity->getRender();
        if (!render->isActive()) {
            continue;
        }

        gameEntity->update(alpha);

        auto* material = render->getMaterial();
        material->apply();

        auto* shader = material->getShader();
        shader->setVec3fv("lightDir", m_lightDir);
        shader->setVec3fv("lightColor", glm::vec3(1.0f));

        render->draw(alpha, ctx);
    }

    // LIGHT
    glm::vec3 lightMarkerPos = SOLDIER_POSITION - (glm::normalize(m_lightDir) * 5.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightMarkerPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));

    auto lightMaterial = ResourceManager::getInstance().getMaterial("light_material");
    auto lightShader = lightMaterial->getShader();
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