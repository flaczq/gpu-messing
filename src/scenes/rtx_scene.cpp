#include "../ecs/entity.hpp"
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

void RtxScene::saveState() {
    Scene::saveState();
}

void RtxScene::fixedUpdate(float fixedt) {
    Scene::fixedUpdate(fixedt);
}

void RtxScene::update(float alpha) {
    Scene::update(alpha);
}

void RtxScene::lateUpdate() {
    Scene::lateUpdate();
}

void RtxScene::end() {
    Scene::end();
}