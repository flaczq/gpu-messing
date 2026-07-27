#include "../configs/log_config.hpp"
#include "../game/camera.h"
#include "../graphics/renderer.h"
#include "../scenes/rtx_scene.h"
#include "../scenes/scene.h"
#include "../scenes/soldier_scene.h"
#include "../utils/enum_utils.hpp"
#include "scene_manager.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>

SceneManager& SceneManager::getInstance() {
	static SceneManager instance;
	return instance;
}

SceneManager::SceneManager() = default;

bool SceneManager::init(Camera* camera) {
	m_camera = camera;

	// default scene
	m_currentScene = std::make_unique<SoldierScene>(m_camera);
	m_currentScene->init();

	return true;
}

void SceneManager::toggleScene() {
	SceneID nextSceneID;

	if (m_currentScene) {
		nextSceneID = Utils::getEnumNext(m_currentScene->getID());
		m_currentScene->end();
	} else {
		nextSceneID = Utils::getEnumFirst(SceneID::SOLDIER);
	}

	// TODO: LOADING
	LOG_D("Changed CurrentScene to: " << Utils::getEnumName(nextSceneID));
	switch (nextSceneID) {
	case SceneID::SOLDIER:
		m_currentScene = std::make_unique<SoldierScene>(m_camera);
		break;
	case SceneID::RTX:
		m_currentScene = std::make_unique<RtxScene>(m_camera);
		break;
		//case SceneID::FPS_GAME:
		//	nextScene = std::make_unique<FpsGameScene>(m_camera);
		//	break;
	}

	m_currentScene->init();
}

void SceneManager::saveState() const {
	if (m_currentScene) {
		m_currentScene->saveState();
	}
}

void SceneManager::fixedUpdate(float fixedt) const {
	if (m_currentScene) {
		m_currentScene->fixedUpdate(fixedt);
	}
}

void SceneManager::update(float alpha) const {
	if (m_currentScene) {
		m_currentScene->update(alpha);
	}
}

void SceneManager::lateUpdate() const {
	if (m_currentScene) {
		m_currentScene->lateUpdate();
	}
}

void SceneManager::end() const {
	if (m_currentScene) {
		m_currentScene->end();
	}
}