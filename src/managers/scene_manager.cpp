#include "../configs/log_config.hpp"
#include "../ecs/registry.h"
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
	// default scene
	m_currentScene = std::make_unique<SoldierScene>();
	m_currentScene->init(registry);

	return true;
}

void SceneManager::toggleScene(Registry& registry) {
	SceneID nextSceneID;
	if (m_currentScene) {
		m_currentScene->end(registry);
		nextSceneID = Utils::Enum::getNext(m_currentScene->getID());
	} else {
		nextSceneID = Utils::Enum::getFirst(SceneID::SOLDIER);
	}

	// TODO: LOADING
	LOG_D("Changed CurrentScene to: " << Utils::Enum::getName(nextSceneID));
	switch (nextSceneID) {
	case SceneID::SOLDIER:
		m_currentScene = std::make_unique<SoldierScene>();
		break;
	case SceneID::RTX:
		m_currentScene = std::make_unique<RtxScene>();
		break;
		//case SceneID::FPS_GAME:
		//	nextScene = std::make_unique<FpsGameScene>();
		//	break;
	}

	m_currentScene->init(registry);
}