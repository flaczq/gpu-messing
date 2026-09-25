#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../ecs/components/ai_component.hpp"
#include "../ecs/components/camera_component.hpp"
#include "../ecs/components/dir_light_movement_component.hpp"
#include "../ecs/components/identity_component.hpp"
#include "../ecs/components/physics_component.hpp"
#include "../ecs/components/player_component.hpp"
#include "../ecs/components/render_component.hpp"
#include "../ecs/components/stats_component.hpp"
#include "../ecs/components/transform_component.hpp"
#include "../ecs/entites/entity.hpp"
#include "../ecs/registry.h"
#include "../graphics/material.h"
#include "../graphics/mesh.h"
#include "../graphics/mesh_generator.h"
#include "../graphics/model.h"
#include "../managers/resource_manager.h"
#include "../utils/color_constants.hpp"
#include "../utils/math_utils.hpp"
#include "../utils/stats_constants.hpp"
#include "scene.h"
#include "soldier_scene.h"
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

SoldierScene::SoldierScene() = default;

bool SoldierScene::init(Registry& registry) {
    //       ▄████████    ▄████████    ▄████████    ▄████████     ███        ▄████████ 
    //      ███    ███   ███    ███   ███    ███   ███    ███ ▀█████████▄   ███    ███ 
    //      ███    ███   ███    █▀    ███    █▀    ███    █▀     ▀███▀▀██   ███    █▀  
    //      ███    ███   ███          ███         ▄███▄▄▄         ███   ▀   ███        
    //    ▀███████████ ▀███████████ ▀███████████ ▀▀███▀▀▀         ███     ▀███████████ 
    //      ███    ███          ███          ███   ███    █▄      ███              ███ 
    //      ███    ███    ▄█    ███    ▄█    ███   ███    ███     ███        ▄█    ███ 
    //      ███    █▀   ▄████████▀   ▄████████▀    ██████████    ▄████▀    ▄████████▀  
    // --- TEXTURES
    ResourceManager::getInstance().loadTexture("window_texture", "../assets/blending_transparent_window.png");
    ResourceManager::getInstance().loadTexture("grass_texture", "../assets/grass.png");
    ResourceManager::getInstance().loadTexture("potato_texture", "../assets/potato.jpg");
    // MODELS
    ResourceManager::getInstance().loadModel("gizmo_model", "../assets/models/Gizmo.fbx");
    ResourceManager::getInstance().loadModel("player_model", "../assets/models/RiggedFpsArms.fbx");
    ResourceManager::getInstance().loadModel("soldier_model", "../assets/models/Soldier.glb");
    ResourceManager::getInstance().loadModel("tank_model", "../assets/models/ShermanTank.glb");
    // SHADERS
    ResourceManager::getInstance().loadShader("simple_shader", "../shaders/simple.vert", "../shaders/simple.frag");
    ResourceManager::getInstance().loadShader("gizmo_shader", "../shaders/gizmo.vert", "../shaders/gizmo.frag");
    ResourceManager::getInstance().loadShader("lambert_shader", "../shaders/lambert.vert", "../shaders/lambert.frag");
    ResourceManager::getInstance().loadShader("model_shader", "../shaders/model.vert", "../shaders/model.frag");
    ResourceManager::getInstance().loadShader("window_shader", "../shaders/window.vert", "../shaders/window.frag");
    ResourceManager::getInstance().loadShader("tex_shader", "../shaders/tex.vert", "../shaders/tex.frag");
    auto simpleShader = ResourceManager::getInstance().getShader("simple_shader");
    auto gizmoShader = ResourceManager::getInstance().getShader("gizmo_shader");
    auto lambertShader = ResourceManager::getInstance().getShader("lambert_shader");
    auto modelShader = ResourceManager::getInstance().getShader("model_shader");
    auto windowShader = ResourceManager::getInstance().getShader("window_shader");
    auto texShader = ResourceManager::getInstance().getShader("tex_shader");
    // MATERIALS
    ResourceManager::getInstance().loadMaterial("floor_material", simpleShader);
    ResourceManager::getInstance().loadMaterial("light_material", simpleShader);
    ResourceManager::getInstance().loadMaterial("grid_material", simpleShader);
    ResourceManager::getInstance().loadMaterial("gizmo_material", gizmoShader);
    ResourceManager::getInstance().loadMaterial("player_material", lambertShader);
    ResourceManager::getInstance().loadMaterial("soldier_material", modelShader);
    ResourceManager::getInstance().loadMaterial("tank_material", modelShader);
    ResourceManager::getInstance().loadMaterial("window_material", windowShader);
    ResourceManager::getInstance().loadMaterial("fbs_material", texShader);

    //       ▄▄▄▄███▄▄▄▄    ▄██████▄  ████████▄     ▄████████  ▄█          ▄████████ 
    //     ▄██▀▀▀███▀▀▀██▄ ███    ███ ███   ▀███   ███    ███ ███         ███    ███ 
    //     ███   ███   ███ ███    ███ ███    ███   ███    █▀  ███         ███    █▀  
    //     ███   ███   ███ ███    ███ ███    ███  ▄███▄▄▄     ███         ███        
    //     ███   ███   ███ ███    ███ ███    ███ ▀▀███▀▀▀     ███       ▀███████████ 
    //     ███   ███   ███ ███    ███ ███    ███   ███    █▄  ███                ███ 
    //     ███   ███   ███ ███    ███ ███   ▄███   ███    ███ ███▌    ▄    ▄█    ███ 
    //      ▀█   ███   █▀   ▀██████▀  ████████▀    ██████████ █████▄▄██  ▄████████▀  
    //                                                        ▀                      
    // --- floor
    glm::vec3 floorSize = glm::vec3(14.0f, 0.0f, 14.0f);
    auto floor = MeshGenerator::createPlane(floorSize.x, floorSize.z);
    auto floorM = std::make_unique<Mesh>(std::move(floor));
    auto floorMM = std::make_shared<Model>("floor_model", std::move(floorM), -floorSize * 0.5f, floorSize * 0.5f);
    // TODO maybe wrap into sigle method call
    ResourceManager::getInstance().addModel(std::move(floorMM));
    // --- light
    auto light = MeshGenerator::createCuboid(2.0f, 2.0f, 2.0f);
    auto lightM = std::make_unique<Mesh>(std::move(light));
    auto lightMM = std::make_shared<Model>("light_model", std::move(lightM));
    ResourceManager::getInstance().addModel(std::move(lightMM));
    // --- grid
    float gridSize = 18.0f;
    float gridStep = 1.0f;
    auto grid = MeshGenerator::createGrid(gridSize, gridStep);
    auto gridM = std::make_unique<Mesh>(std::move(grid));
    auto gridMM = std::make_shared<Model>("grid_model", std::move(gridM));
    ResourceManager::getInstance().addModel(std::move(gridMM));
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
    // --- framebuffer screen
    auto potatoTexture = ResourceManager::getInstance().getTexture("potato_texture");
    auto fbs = MeshGenerator::createPlane(2.0f, 2.0f, potatoTexture);
    auto fbsM = std::make_unique<Mesh>(std::move(fbs));
    auto fbsMM = std::make_shared<Model>("fbs_model", std::move(fbsM));
    ResourceManager::getInstance().addModel(std::move(fbsMM));

    //       ▄████████ ███▄▄▄▄       ███      ▄█      ███      ▄█     ▄████████    ▄████████ 
    //      ███    ███ ███▀▀▀██▄ ▀█████████▄ ███  ▀█████████▄ ███    ███    ███   ███    ███ 
    //      ███    █▀  ███   ███    ▀███▀▀██ ███▌    ▀███▀▀██ ███▌   ███    █▀    ███    █▀  
    //     ▄███▄▄▄     ███   ███     ███   ▀ ███▌     ███   ▀ ███▌  ▄███▄▄▄       ███        
    //    ▀▀███▀▀▀     ███   ███     ███     ███▌     ███     ███▌ ▀▀███▀▀▀     ▀███████████ 
    //      ███    █▄  ███   ███     ███     ███      ███     ███    ███    █▄           ███ 
    //      ███    ███ ███   ███     ███     ███      ███     ███    ███    ███    ▄█    ███ 
    //      ██████████  ▀█   █▀     ▄████▀   █▀      ▄████▀   █▀     ██████████  ▄████████▀  
    // --- floor
    auto floorModel = ResourceManager::getInstance().getModel("floor_model");
    auto floorMaterial = ResourceManager::getInstance().getMaterial("floor_material");
    if (floorModel && floorMaterial) {
        // MATERIAL UNIFORMS
        floorMaterial->addBoolUniform("hasMatColor", true);
        floorMaterial->addVec3Uniform("matColor", Constants::Color::NATGREEN);
        Entity floorE = registry.createEntity();
        registry.addComponent<IdentityComponent>(floorE, "floor");
        registry.addComponent<TransformComponent>(floorE, glm::vec3(floorSize.x / 2.0f + 2.0f, 0.0f, floorSize.z / 2.0f + 2.0f));
        registry.addComponent<PhysicsComponent>(floorE, floorModel->getAABBMin(), floorModel->getAABBMax(), true);
        registry.addComponent<RenderComponent>(floorE, floorModel, floorMaterial);
    }
    // --- light
    auto lightModel = ResourceManager::getInstance().getModel("light_model");
    auto lightMaterial = ResourceManager::getInstance().getMaterial("light_material");
    if (lightModel && lightMaterial) {
        lightMaterial->addBoolUniform("hasMatColor", true);
        lightMaterial->addVec3Uniform("matColor", Constants::Color::WHITE);
        Entity lightE = registry.createEntity();
        registry.addComponent<IdentityComponent>(lightE, "light");
        registry.addComponent<TransformComponent>(lightE, glm::vec3(3.0f, 0.0f, 3.0f), glm::quat(), glm::vec3(0.2f));
        registry.addComponent<DirLightMovementComponent>(lightE, -glm::vec3(3.0f, 0.0f, 3.0f), Constants::Color::WHITE);
        registry.addComponent<RenderComponent>(lightE, lightModel, lightMaterial);
    }
    // --- grid
    auto gridModel = ResourceManager::getInstance().getModel("grid_model");
    auto gridMaterial = ResourceManager::getInstance().getMaterial("grid_material");
    if (gridModel && gridMaterial) {
        // same shader so it has to be set back to 'false'
        gridMaterial->addBoolUniform("hasMatColor", false);
        Entity gridE = registry.createEntity();
        registry.addComponent<IdentityComponent>(gridE, "grid");
        registry.addComponent<TransformComponent>(gridE, glm::vec3(gridSize / 2.0f));
        registry.addComponent<RenderComponent>(gridE, gridModel, gridMaterial);
    }
    // --- gizmo
    /*auto gizmoModel = ResourceManager::getInstance().getModel("gizmo_model");
    auto gizmoMaterial = ResourceManager::getInstance().getMaterial("gizmo_material");
    if (gizmoModel && gizmoMaterial) {
        Entity gizmoE = registry.createEntity();
        registry.addComponent<IdentityComponent>(gizmoE, "gizmo");
        registry.addComponent<TransformComponent>(gizmoE, glm::vec3(0.0f), glm::quat(), glm::vec3(7.5f));
        registry.addComponent<RenderComponent>(gizmoE, gizmoModel, gizmoMaterial);
    }*/
    // --- fps arms
    auto playerModel = ResourceManager::getInstance().getModel("player_model");
    auto playerMaterial = ResourceManager::getInstance().getMaterial("player_material");
    if (playerModel && playerMaterial) {
        Entity playerE = registry.createEntity();
        registry.addComponent<IdentityComponent>(playerE, "player");
        registry.addComponent<TransformComponent>(playerE, glm::vec3(1.7f, 0.0f, 17.0f), glm::quat(), glm::vec3(0.2f));
        registry.addComponent<PhysicsComponent>(playerE, glm::vec3(-0.25f), glm::vec3(0.25f), false, 5.0f, PhysicsLayer::TOP);
        registry.addComponent<PlayerComponent>(playerE);
        registry.addComponent<CameraComponent>(playerE);
        registry.addComponent<RenderComponent>(playerE, playerModel, playerMaterial, RenderQueueType::TOP_LAYER);
    }
    // --- soldier
    auto soldierModel = ResourceManager::getInstance().getModel("soldier_model");
    auto soldierMaterial = ResourceManager::getInstance().getMaterial("soldier_material");
    if (soldierModel && soldierMaterial) {
        // MATERIAL UNIFORMS
        //soldierMaterial->addVec3Uniform("lightColor", glm::vec3(1.0f));
        float spacing = 1.5f;
        for (unsigned int i{}; i < 49; i++) {
            unsigned int row = i / 7;
            unsigned int col = i % 7;
            glm::vec3 sPos = glm::vec3(3.0f, 0.01f, 3.0f) + glm::vec3(col * spacing, 0.0f, row * spacing);
            glm::quat sRotQ = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            Entity soldierE = registry.createEntity();
            registry.addComponent<IdentityComponent>(soldierE, "soldier_" + std::to_string(i), LayerID::SOLDIERS);
            registry.addComponent<TransformComponent>(soldierE, sPos, sRotQ, glm::vec3(100.0f));
            registry.addComponent<PhysicsComponent>(soldierE, soldierModel->getAABBMin(), soldierModel->getAABBMax());
            registry.addComponent<RenderComponent>(soldierE, soldierModel, soldierMaterial);
            if (i == 30) {
                registry.addComponent<AIComponent>(soldierE, 1.3f);
            }
        }
    }
    // --- sherman tank
    /*auto tankModel = ResourceManager::getInstance().getModel("tank_model");
    auto tankMaterial = ResourceManager::getInstance().getMaterial("tank_material");
    if (tankModel && tankMaterial) {
        glm::quat tRotQ = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        Entity tankE = registry.createEntity();
        registry.addComponent<IdentityComponent>(tankE, "tank");
        registry.addComponent<TransformComponent>(tankE, glm::vec3(15.0f, 0.01f, 8.0f), tRotQ, glm::vec3(5.0f));
        registry.addComponent<RenderComponent>(tankE, tankModel, tankMaterial);
        registry.addComponent<PhysicsComponent>(tankE, tankModel->getAABBMin(), tankModel->getAABBMax());
    }*/
    // --- stencil boxes
    auto stencilBoxModel = ResourceManager::getInstance().getModel("stencil_box_model");
    auto stencilBox1Material = ResourceManager::getInstance().getMaterial("window_material");
    auto stencilBox2Material = ResourceManager::getInstance().getMaterial("light_material");
    if (stencilBoxModel && stencilBox1Material && stencilBox2Material) {
        Entity stencilBoxE = registry.createEntity();
        registry.addComponent<IdentityComponent>(stencilBoxE, "stencil_box_1");
        registry.addComponent<TransformComponent>(stencilBoxE, glm::vec3(5.0f, 1.0f, 6.0f));
        //registry.addComponent<RenderComponent>(stencilBoxE, stencilBoxModel, stencilBox1Material, RenderQueueType::STENCIL);
        stencilBoxE = registry.createEntity();
        registry.addComponent<IdentityComponent>(stencilBoxE, "stencil_box_2");
        registry.addComponent<TransformComponent>(stencilBoxE, glm::vec3(5.0f, 1.0f, 6.0f), glm::quat(), glm::vec3(1.1f));
        //registry.addComponent<RenderComponent>(stencilBoxE, stencilBoxModel, stencilBox2Material, RenderQueueType::OUTLINE);
    }
    // --- window
    auto windowModel = ResourceManager::getInstance().getModel("window_model");
    auto windowMaterial = ResourceManager::getInstance().getMaterial("window_material");
    if (windowModel && windowMaterial) {
        Entity windowE = registry.createEntity();
        registry.addComponent<IdentityComponent>(windowE, "window_1");
        registry.addComponent<TransformComponent>(windowE, glm::vec3(-2.0f, 1.0f, 3.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        registry.addComponent<RenderComponent>(windowE, windowModel, windowMaterial, RenderQueueType::BLENDING);
        windowE = registry.createEntity();
        registry.addComponent<IdentityComponent>(windowE, "window_2");
        registry.addComponent<TransformComponent>(windowE, glm::vec3(-3.0f, 1.0f, 4.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        registry.addComponent<RenderComponent>(windowE, windowModel, windowMaterial, RenderQueueType::BLENDING);
    }
    // --- grass
    auto grassModel = ResourceManager::getInstance().getModel("grass_model");
    //auto windowMaterial = ResourceManager::getInstance().getMaterial("window_material");
    if (grassModel && windowMaterial) {
        Entity grassE = registry.createEntity();
        registry.addComponent<IdentityComponent>(grassE, "grass_1");
        registry.addComponent<TransformComponent>(grassE, glm::vec3(-2.0f, 1.0f, 5.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        registry.addComponent<RenderComponent>(grassE, grassModel, windowMaterial, RenderQueueType::BLENDING);
        grassE = registry.createEntity();
        registry.addComponent<IdentityComponent>(grassE, "grass_2");
        registry.addComponent<TransformComponent>(grassE, glm::vec3(-2.0f, 1.0f, 2.0f), glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        registry.addComponent<RenderComponent>(grassE, grassModel, windowMaterial, RenderQueueType::BLENDING);
    }
    // --- framebuffer screen
    auto fbsModel = ResourceManager::getInstance().getModel("fbs_model");
    auto fbsMaterial = ResourceManager::getInstance().getMaterial("fbs_material");
    if (fbsModel && fbsMaterial) {
        Entity fbsE = registry.createEntity();
        registry.addComponent<IdentityComponent>(fbsE, "fbs");
        registry.addComponent<TransformComponent>(fbsE, glm::vec3(0.0f));
        registry.addComponent<RenderComponent>(fbsE, fbsModel, fbsMaterial);
    }

    return Scene::init(registry);
}

void SoldierScene::end(Registry& registry) {
    Scene::end(registry);
}