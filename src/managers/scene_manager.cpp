#include "../configs/log_config.hpp"
#include "../ecs/registry.h"
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

bool SceneManager::init(Registry& registry) {
	m_registry = registry;

	// default scene
	m_currentScene = std::make_unique<SoldierScene>(m_registry);
	m_currentScene->init();

	return true;
}

void SceneManager::toggleScene() {
	SceneID nextSceneID;

	if (m_currentScene) {
		nextSceneID = Utils::Enum::getNext(m_currentScene->getID());
		m_currentScene->end();
	} else {
		nextSceneID = Utils::Enum::getFirst(SceneID::SOLDIER);
	}

	// TODO: LOADING
	LOG_D("Changed CurrentScene to: " << Utils::Enum::getName(nextSceneID));
	switch (nextSceneID) {
	case SceneID::SOLDIER:
		m_currentScene = std::make_unique<SoldierScene>(m_registry, m_physicsSystem, m_camera);
		break;
	case SceneID::RTX:
		m_currentScene = std::make_unique<RtxScene>(m_registry, m_physicsSystem, m_camera);
		break;
		//case SceneID::FPS_GAME:
		//	nextScene = std::make_unique<FpsGameScene>(m_registry, m_physicsSystem, m_camera);
		//	break;
	}

	m_currentScene->init();
}