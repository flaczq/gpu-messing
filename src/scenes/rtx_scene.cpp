#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../managers/resource_manager.h"
#include "rtx_scene.h"
#include "scene.h"
#include <memory>

RtxScene::RtxScene(Camera* camera)
	: m_camera(camera)
{
}

void RtxScene::init() {
    // add shinny ballz
    Scene::init();
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