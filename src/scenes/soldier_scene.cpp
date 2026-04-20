#include "../components/dir_light_movement_component.h"
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
#include "../graphics/renderer.h"
#include "../managers/resource_manager.h"
#include "scene.h"
#include "soldier_scene.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

SoldierScene::SoldierScene(Camera* camera, Renderer* renderer)
    : m_camera(camera),
      m_renderer(renderer)
{
}

void SoldierScene::init() {
    auto& resourceManager = ResourceManager::getInstance();
    // MODELS
    // --- floor
    auto floor = MeshGenerator::createPlane(16.0f, 15.0f);
    auto floorM = std::make_unique<Mesh>(std::move(floor));
    auto floorMM = std::make_shared<Model>(std::move(floorM));
    resourceManager.addModel("floor_model", std::move(floorMM));
    // --- soldier
    resourceManager.loadModel("soldier_model", "../assets/models/Soldier.glb");
    // --- light
    auto light = MeshGenerator::createCuboid(2.0f, 2.0f, 2.0f);
    auto lightM = std::make_unique<Mesh>(std::move(light));
    auto lightMM = std::make_shared<Model>(std::move(lightM));
    resourceManager.addModel("light_model", std::move(lightMM));
    // MATERIALS with SHADERS
    resourceManager.loadMaterial("soldier_material", "../shaders/model.vert", "../shaders/model.frag");
    resourceManager.loadMaterial("light_material", "../shaders/light.vert", "../shaders/light.frag");
    resourceManager.loadMaterial("floor_material", "../shaders/lambert.vert", "../shaders/lambert.frag");

    // FLOOR
    auto floorModel = resourceManager.getModel("floor_model");
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
    auto lightModel = resourceManager.getModel("light_model");
    auto lightMaterial = resourceManager.getMaterial("light_material");
    if (lightModel && lightMaterial) {
        glm::quat lRotQ = glm::angleAxis(LIGHT_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
        auto lightGO = std::make_unique<GameEntity>("light");
        lightGO->addComponent<TransformComponent>(LIGHT_POSITION, lRotQ, LIGHT_SCALE);
        lightGO->addComponent<RenderComponent>(lightModel, lightMaterial);
        lightGO->addComponent<DirLightMovementComponent>(m_renderer);
        lightGO->init();
        m_gameEntities.push_back(std::move(lightGO));
    }
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
    // SOLDIER
    for (auto& gameEntity : m_gameEntities) {
        if (!gameEntity->checkStatus()) {
            continue;
        }
        auto* transform = gameEntity->getTransform();
        if (!transform->isActive()) {
            continue;
        }

        gameEntity->fixedUpdate(fixedt);

        /*if (transform->getOwner()->getName().starts_with("soldier_")) {
            float time = static_cast<float>(glfwGetTime());
            glm::quat newRot = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(glm::radians(90.0f) * time, glm::vec3(0.0f, 0.0f, 1.0f));
            auto nr = transform->getOwner()->getName().substr(transform->getOwner()->getName().find_last_of('_') + 1);
            if (std::stoi(nr) % 3 == 0) {
                transform->setRotation(newRot);
            }
        }*/
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
    RenderLight* renderLight = m_renderer->getRenderLight();
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
        shader->setVec3fv("lightDir", renderLight->direction);
        shader->setVec3fv("lightColor", renderLight->color);

        render->draw(alpha, ctx);
    }
}

void SoldierScene::end() {
    for (auto& gameEntity : m_gameEntities) {
        /*if (!gameEntity->checkStatus()) {
            continue;
        }*/

        gameEntity->end();
    }
}