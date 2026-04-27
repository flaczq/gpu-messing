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

SoldierScene::SoldierScene(Camera* camera)
    : m_camera(camera)
{
}

void SoldierScene::init() {
    auto& resourceManager = ResourceManager::getInstance();
    // MODELS
    // --- floor
    auto floor = MeshGenerator::createPlane(16.0f, 15.0f);
    auto floorM = std::make_unique<Mesh>(std::move(floor));
    auto floorMM = std::make_shared<Model>("floor_model", std::move(floorM));
    resourceManager.addModel(std::move(floorMM));
    // --- gizmo
    auto gizmo = MeshGenerator::createCuboid(2.0f, 2.0f, 2.0f);
    auto gizmoM = std::make_unique<Mesh>(std::move(gizmo));
    auto gizmoMM = std::make_shared<Model>("gizmo_model", std::move(gizmoM));
    resourceManager.addModel(std::move(gizmoMM));
    // --- light
    auto light = MeshGenerator::createCuboid(2.0f, 2.0f, 2.0f);
    auto lightM = std::make_unique<Mesh>(std::move(light));
    auto lightMM = std::make_shared<Model>("light_model", std::move(lightM));
    resourceManager.addModel(std::move(lightMM));
    // --- soldier
    resourceManager.loadModel("soldier_model", "../assets/models/Soldier.glb");
    // MATERIALS with SHADERS
    resourceManager.loadMaterial("floor_material", "../shaders/lambert.vert", "../shaders/lambert.frag");
    resourceManager.loadMaterial("gizmo_material", "../shaders/gizmo.vert", "../shaders/gizmo.frag");
    resourceManager.loadMaterial("light_material", "../shaders/light.vert", "../shaders/light.frag");
    resourceManager.loadMaterial("soldier_material", "../shaders/model.vert", "../shaders/model.frag");

    // FLOOR
    auto floorModel = resourceManager.getModel("floor_model");
    auto floorMaterial = resourceManager.getMaterial("floor_material");
    if (floorModel && floorMaterial) {
        // MATERIAL UNIFORMS
        //floorMaterial->addVec3Uniform("lightColor", glm::vec3(1.0f));

        auto floorGO = std::make_unique<GameEntity>("floor");
        floorGO->setSolid(true);
        floorGO->addComponent<TransformComponent>(FLOOR_POSITION);
        floorGO->addComponent<RenderComponent>(floorModel, floorMaterial);
        floorGO->init();
        m_gameEntities.push_back(std::move(floorGO));
    }

    // GIZMO
    auto gizmoModel = resourceManager.getModel("gizmo_model");
    auto gizmoMaterial = resourceManager.getMaterial("soldier_material");
    if (gizmoModel && gizmoMaterial) {
        //gizmoMaterial->setStencilRequired(true);
        auto gizmoGO = std::make_unique<GameEntity>("gizmo");
        gizmoGO->setRendererQueueType(RendererQueueType::STENCIL);
        gizmoGO->setSolid(true);
        gizmoGO->addComponent<TransformComponent>(glm::vec3(0.0f));
        gizmoGO->addComponent<RenderComponent>(gizmoModel, gizmoMaterial);
        gizmoGO->init();
        m_gameEntities.push_back(std::move(gizmoGO));
    }
    gizmoModel = resourceManager.getModel("gizmo_model");
    gizmoMaterial = resourceManager.getMaterial("gizmo_material");
    if (gizmoModel && gizmoMaterial) {
        auto gizmoGO = std::make_unique<GameEntity>("outline");
        gizmoGO->setRendererQueueType(RendererQueueType::OUTLINE);
        gizmoGO->setSolid(true);
        gizmoGO->addComponent<TransformComponent>(glm::vec3(0.0f), glm::quat(), glm::vec3(1.1f));
        gizmoGO->addComponent<RenderComponent>(gizmoModel, gizmoMaterial);
        gizmoGO->init();
        m_gameEntities.push_back(std::move(gizmoGO));
    }

    // LIGHT
    auto lightModel = resourceManager.getModel("light_model");
    auto lightMaterial = resourceManager.getMaterial("light_material");
    if (lightModel && lightMaterial) {
        auto lightGO = std::make_unique<GameEntity>("light");
        lightGO->setAbstract(true);
        lightGO->addComponent<TransformComponent>(LIGHT_POSITION, glm::quat(), LIGHT_SCALE);
        lightGO->addComponent<RenderComponent>(lightModel, lightMaterial);
        lightGO->addComponent<DirLightMovementComponent>();
        lightGO->init();
        m_gameEntities.push_back(std::move(lightGO));
    }

    // SOLDIER
    auto soldierModel = resourceManager.getModel("soldier_model");
    auto soldierMaterial = resourceManager.getMaterial("soldier_material");
    if (soldierModel && soldierMaterial) {
        // MATERIAL UNIFORMS
        //soldierMaterial->addVec3Uniform("lightColor", glm::vec3(1.0f));

        float spacing = 1.5f;
        for (size_t i{}; i < 49; i++) {
            size_t row = i / 7;
            size_t col = i % 7;
            glm::vec3 sPos = SOLDIER_POSITION + glm::vec3(col * spacing, 0.0f, row * spacing);
            glm::quat sRotQ = glm::angleAxis(SOLDIER_ROTATION, glm::vec3(1.0f, 0.0f, 0.0f));
            auto soldierGO = std::make_unique<GameEntity>("soldier_" + std::to_string(i));
            soldierGO->addComponent<TransformComponent>(sPos, sRotQ, SOLDIER_SCALE);
            soldierGO->addComponent<RenderComponent>(soldierModel, soldierMaterial);
            soldierGO->init();
            m_gameEntities.push_back(std::move(soldierGO));
        }
    }

    // FIXME hardcoded max: 100
    m_aliveGameEntities.reserve(100);
    m_deadGameEntities.reserve(100);

    for (auto& gameEntity : m_gameEntities) {
        if (gameEntity->isAlive() && !gameEntity->isPendingDeath()) {
            m_aliveGameEntities.push_back(gameEntity.get());
        } else {
            m_deadGameEntities.push_back(gameEntity.get());
        }
    }
}

void SoldierScene::saveState() {
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        auto* transform = aliveGameEntity->getTransform();
        transform->saveState();
    }
}

void SoldierScene::fixedUpdate(float fixedt) {
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        aliveGameEntity->fixedUpdate(fixedt);

        /*if (aliveGameEntity->getName().starts_with("soldier_15")) {
            aliveGameEntity->destroy();
        }*/
    }
}

void SoldierScene::update(float alpha) {
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        aliveGameEntity->update(alpha);

        auto* transform = aliveGameEntity->getTransform();
        auto* render = aliveGameEntity->getRender();
        auto* model = render->getModel();
        auto* material = render->getMaterial();
        glm::mat4 modelMatrix = transform->getInterpolatedModelMatrix(alpha);
        glm::mat3 normalMatrix = transform->getNormalMatrix(modelMatrix);
        transform->setDirty(false);
        RendererQueueType queueType = aliveGameEntity->getRendererQueueType();
        RendererCommand command = {
            model,
            material,
            modelMatrix,
            normalMatrix
        };
        Renderer::getInstance().registerInQueue(queueType, command);
    }

    Renderer::getInstance().flush();
}

void SoldierScene::lateUpdate() {
    for (int i = 0; i < m_aliveGameEntities.size(); /*i++*/) {
        //LOG_D("CHECKING " << m_aliveGameEntities[i]->getName());
        if (m_aliveGameEntities[i]->isPendingDeath()) {
            //LOG_D("DEAD " << m_aliveGameEntities[i]->getName());
            GameEntity* gameEntity = m_aliveGameEntities[i];
            gameEntity->setPendingDeath(false);
            gameEntity->setAlive(false);
            m_deadGameEntities.push_back(gameEntity);

            // fast delete (swap & pop)
            m_aliveGameEntities[i] = m_aliveGameEntities.back();
            m_aliveGameEntities.pop_back();
            // 'i' -> new element from back
        } else {
            i++;
        }
    }
    //LOG_D("ALL " << m_gameEntities.size());
    //LOG_D("ALIVE " << m_aliveGameEntities.size());
    //LOG_D("DEAD " << m_deadGameEntities.size());
}

void SoldierScene::end() {
    for (auto& gameEntity : m_gameEntities) {
        gameEntity->end();
    }

    m_deadGameEntities.clear();
    m_aliveGameEntities.clear();
    m_gameEntities.clear();
}