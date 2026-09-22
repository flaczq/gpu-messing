#include "../ecs/entites/entity.hpp"
#include "../ecs/registry.h"
#include "rtx_scene.h"
#include "scene.h"

RtxScene::RtxScene() = default;

bool RtxScene::init(Registry& registry) {
    // add shinny ballz
    //m_registry.createEntity(shinyballz);

    return Scene::init(registry);
}

void RtxScene::end(Registry& registry) {
    Scene::end(registry);
}