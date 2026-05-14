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
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

SoldierScene::SoldierScene(Camera* camera)
    : m_camera(camera)
{
}

void SoldierScene::init() {
    //    ┓ ┏┓┏┓┳┓  ┳┓┏┓┏┳┓┏┓
    //    ┃ ┃┃┣┫┃┃  ┃┃┣┫ ┃ ┣┫
    //    ┗┛┗┛┛┗┻┛  ┻┛┛┗ ┻ ┛┗
    //                       
    // TEXTURES
    ResourceManager::getInstance().loadTexture("window_texture", "../assets/blending_transparent_window.png");
    ResourceManager::getInstance().loadTexture("grass_texture", "../assets/grass.png");
    // MODELS
    ResourceManager::getInstance().loadModel("gizmo_model", "../assets/models/Gizmo.fbx");
    ResourceManager::getInstance().loadModel("soldier_model", "../assets/models/Soldier.glb");
    ResourceManager::getInstance().loadModel("arms_model", "../assets/models/RiggedFpsArms.fbx");
    // MATERIALS with SHADERS
    ResourceManager::getInstance().loadMaterial("floor_material", "../shaders/light.vert", "../shaders/light.frag");
    ResourceManager::getInstance().loadMaterial("light_material", "../shaders/light.vert", "../shaders/light.frag");
    ResourceManager::getInstance().loadMaterial("gizmo_material", "../shaders/gizmo.vert", "../shaders/gizmo.frag");
    ResourceManager::getInstance().loadMaterial("arms_material", "../shaders/lambert.vert", "../shaders/lambert.frag");
    ResourceManager::getInstance().loadMaterial("soldier_material", "../shaders/model.vert", "../shaders/model.frag");
    ResourceManager::getInstance().loadMaterial("window_material", "../shaders/window.vert", "../shaders/window.frag");

    // MODELS
    // --- floor
    auto floor = MeshGenerator::createPlane(16.0f, 15.0f);
    auto floorM = std::make_unique<Mesh>(std::move(floor));
    auto floorMM = std::make_shared<Model>("floor_model", std::move(floorM));
    ResourceManager::getInstance().addModel(std::move(floorMM));
    // --- light
    auto light = MeshGenerator::createCuboid(2.0f, 2.0f, 2.0f);
    auto lightM = std::make_unique<Mesh>(std::move(light));
    auto lightMM = std::make_shared<Model>("light_model", std::move(lightM));
    ResourceManager::getInstance().addModel(std::move(lightMM));
    // --- stencil boxes
    auto stencilBox = MeshGenerator::createCuboid(3.0f, 3.0f, 3.0f);
    auto stencilBoxM = std::make_unique<Mesh>(std::move(stencilBox));
    auto stencilBoxMM = std::make_shared<Model>("stencil_box_model", std::move(stencilBoxM));
    ResourceManager::getInstance().addModel(std::move(stencilBoxMM));
    // --- blending window
    auto windowTexture = ResourceManager::getInstance().getTexture("window_texture");
    auto window = MeshGenerator::createPlane(2.0f, 2.0f, windowTexture);
    auto windowM = std::make_unique<Mesh>(std::move(window));
    auto windowMM = std::make_shared<Model>("window_model", std::move(windowM));
    ResourceManager::getInstance().addModel(std::move(windowMM));
    // --- blending grass
    auto grassTexture = ResourceManager::getInstance().getTexture("grass_texture");
    auto grass = MeshGenerator::createPlane(1.0f, 1.0f, grassTexture);
    auto grassM = std::make_unique<Mesh>(std::move(grass));
    auto grassMM = std::make_shared<Model>("grass_model", std::move(grassM));
    ResourceManager::getInstance().addModel(std::move(grassMM));

    // FLOOR
    auto floorModel = ResourceManager::getInstance().getModel("floor_model");
    auto floorMaterial = ResourceManager::getInstance().getMaterial("floor_material");
    if (floorModel && floorMaterial) {
        // MATERIAL UNIFORMS
        floorMaterial->addVec3Uniform("matColor", glm::vec3(0.3f, 0.8f, 0.3f));

        auto floorGO = std::make_unique<GameEntity>("floor");
        //floorGO->setRendererQueueType(RendererQueueType::OPAQUE);
        floorGO->setSolid(true);
        floorGO->addComponent<TransformComponent>(FLOOR_POSITION);
        floorGO->addComponent<RenderComponent>(floorModel, floorMaterial);
        floorGO->init();
        m_gameEntities.push_back(std::move(floorGO));
    }
    // LIGHT
    auto lightModel = ResourceManager::getInstance().getModel("light_model");
    auto lightMaterial = ResourceManager::getInstance().getMaterial("light_material");
    if (lightModel && lightMaterial) {
        lightMaterial->addVec3Uniform("matColor", glm::vec3(1.0f));

        auto lightGO = std::make_unique<GameEntity>("light");
        //lightGO->setAbstract(true);
        lightGO->addComponent<TransformComponent>(LIGHT_POSITION, glm::quat(), LIGHT_SCALE);
        lightGO->addComponent<RenderComponent>(lightModel, lightMaterial);
        lightGO->addComponent<DirLightMovementComponent>();
        lightGO->init();
        m_gameEntities.push_back(std::move(lightGO));
    }
    // GIZMO
    auto gizmoModel = ResourceManager::getInstance().getModel("gizmo_model");
    auto gizmoMaterial = ResourceManager::getInstance().getMaterial("gizmo_material");
    if (gizmoModel && gizmoMaterial) {
        auto gizmoGO = std::make_unique<GameEntity>("gizmo");
        gizmoGO->setSolid(true);
        gizmoGO->addComponent<TransformComponent>(glm::vec3(0.0f), glm::quat(), GIZMO_SCALE);
        gizmoGO->addComponent<RenderComponent>(gizmoModel, gizmoMaterial);
        gizmoGO->init();
        m_gameEntities.push_back(std::move(gizmoGO));
    }
    // FPS ARMS
    auto armsModel = ResourceManager::getInstance().getModel("arms_model");
    auto armsMaterial = ResourceManager::getInstance().getMaterial("arms_material");
    if (armsModel && armsMaterial) {
        auto armsGO = std::make_unique<GameEntity>("arms");
        armsGO->addComponent<TransformComponent>(glm::vec3(0.0f));
        armsGO->addComponent<RenderComponent>(armsModel, armsMaterial);
        armsGO->init();
        m_gameEntities.push_back(std::move(armsGO));
    }
    // SOLDIER
    auto soldierModel = ResourceManager::getInstance().getModel("soldier_model");
    auto soldierMaterial = ResourceManager::getInstance().getMaterial("soldier_material");
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
    // STENCIL BOXES
    /*auto stencilBoxModel = ResourceManager::getInstance().getModel("stencil_box_model");
    auto stencilBox1Material = ResourceManager::getInstance().getMaterial("window_material");
    auto stencilBox2Material = ResourceManager::getInstance().getMaterial("light_material");
    if (stencilBoxModel && stencilBox1Material && stencilBox2Material) {
        auto stencilBoxGO = std::make_unique<GameEntity>("stencil_box1");
        stencilBoxGO->setRendererQueueType(RendererQueueType::STENCIL);
        stencilBoxGO->setSolid(true);
        stencilBoxGO->addComponent<TransformComponent>(glm::vec3(5.0f, 1.0f, 6.0f));
        stencilBoxGO->addComponent<RenderComponent>(stencilBoxModel, stencilBox1Material);
        stencilBoxGO->init();
        m_gameEntities.push_back(std::move(stencilBoxGO));
        stencilBoxGO = std::make_unique<GameEntity>("stencil_box2");
        stencilBoxGO->setRendererQueueType(RendererQueueType::OUTLINE);
        stencilBoxGO->setSolid(true);
        stencilBoxGO->addComponent<TransformComponent>(glm::vec3(5.0f, 1.0f, 6.0f), glm::quat(), glm::vec3(1.1f));
        stencilBoxGO->addComponent<RenderComponent>(stencilBoxModel, stencilBox2Material);
        stencilBoxGO->init();
        m_gameEntities.push_back(std::move(stencilBoxGO));
    }*/
    // WINDOW
    auto windowModel = ResourceManager::getInstance().getModel("window_model");
    auto windowMaterial = ResourceManager::getInstance().getMaterial("window_material");
    if (windowModel && windowMaterial) {
        auto windowGO = std::make_unique<GameEntity>("window1");
        windowGO->setRendererQueueType(RendererQueueType::BLENDING);
        windowGO->addComponent<TransformComponent>(glm::vec3(-2.0f, 1.0f, 3.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        windowGO->addComponent<RenderComponent>(windowModel, windowMaterial);
        windowGO->init();
        m_gameEntities.push_back(std::move(windowGO));
        windowGO = std::make_unique<GameEntity>("window2");
        windowGO->setRendererQueueType(RendererQueueType::BLENDING);
        windowGO->addComponent<TransformComponent>(glm::vec3(-3.0f, 1.0f, 4.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        windowGO->addComponent<RenderComponent>(windowModel, windowMaterial);
        windowGO->init();
        m_gameEntities.push_back(std::move(windowGO));
    }
    // GRASS
    auto grassModel = ResourceManager::getInstance().getModel("grass_model");
    //auto windowMaterial = ResourceManager::getInstance().getMaterial("window_material");
    if (grassModel && windowMaterial) {
        auto grassGO = std::make_unique<GameEntity>("grass1");
        grassGO->setRendererQueueType(RendererQueueType::BLENDING);
        grassGO->addComponent<TransformComponent>(glm::vec3(-2.0f, 1.0f, 5.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        grassGO->addComponent<RenderComponent>(grassModel, windowMaterial);
        grassGO->init();
        m_gameEntities.push_back(std::move(grassGO));
        grassGO = std::make_unique<GameEntity>("grass2");
        grassGO->setRendererQueueType(RendererQueueType::BLENDING);
        grassGO->addComponent<TransformComponent>(glm::vec3(-2.0f, 1.0f, 2.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        grassGO->addComponent<RenderComponent>(grassModel, windowMaterial);
        grassGO->init();
        m_gameEntities.push_back(std::move(grassGO));
    }

    // FIXME hardcoded max: 100
    m_aliveGameEntities.reserve(100);
    m_deadGameEntities.reserve(100);

    //bool isStencilReqd = false;
    //bool isOutlineReqd = false;
    //bool isBlendingReqd = false;
    for (auto& gameEntity : m_gameEntities) {
        if (gameEntity->isAlive() && !gameEntity->isPendingDeath()) {
            m_aliveGameEntities.push_back(gameEntity.get());

            //if (gameEntity->getRendererQueueType() == RendererQueueType::STENCIL) {
            //    isStencilReqd = true;
            //}
            //if (gameEntity->getRendererQueueType() == RendererQueueType::OUTLINE) {
            //    isOutlineReqd = true;
            //}
            //if (gameEntity->getRendererQueueType() == RendererQueueType::BLENDING) {
            //    isBlendingReqd = true;
            //}
        } else {
            m_deadGameEntities.push_back(gameEntity.get());
        }
    }

    // first frame Renderer params
    // reqd if i need it at the very start of a frame before update()
    //Renderer::getInstance().setStencilReqd(isStencilReqd || isOutlineReqd);
    //Renderer::getInstance().setBlendingReqd(isBlendingReqd);
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

// renderrring
void SoldierScene::update(float alpha) {
    //bool isStencilReqd = false;
    //bool isOutlineReqd = false;
    //bool isBlendingReqd = false;
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        aliveGameEntity->update(alpha);

        auto* transform = aliveGameEntity->getTransform();
        auto* render = aliveGameEntity->getRender();
        if (!render) {
            return;
        }

        auto* model = render->getModel();
        auto* material = render->getMaterial();
        glm::mat4 modelMatrix = transform->getInterpolatedModelMatrix(alpha);
        glm::mat3 normalMatrix = transform->getNormalMatrix(modelMatrix);
        glm::vec3 position = transform->getPosition();
        transform->setDirty(false);
        RendererQueueType queueType = aliveGameEntity->getRendererQueueType();
        RendererCommand command = {
            model,
            material,
            modelMatrix,
            normalMatrix,
            position
        };
        Renderer::getInstance().registerInQueue(queueType, command);

        //if (queueType == RendererQueueType::STENCIL) {
        //    isStencilReqd = true;
        //}
        //if (queueType == RendererQueueType::OUTLINE) {
        //    isOutlineReqd = true;
        //}
        //if (queueType == RendererQueueType::BLENDING) {
        //    isBlendingReqd = true;
        //}
    }

    // next frame Renderer param
    // not reqd because i can check if queue is empty()
    //Renderer::getInstance().setStencilReqd(isStencilReqd || isOutlineReqd);
    //Renderer::getInstance().setBlendingReqd(isBlendingReqd);

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