#include "../ecs/entites/entity.hpp"
#include "../ecs/registry.h"
#include "rtx_scene.h"
#include "scene.h"

RtxScene::RtxScene(Registry& registry)
	: Scene(registry)
{
}

bool RtxScene::init() {
    // add shinny ballz
    //m_registry.createEntity(shinyballz);
    return Scene::init();
}