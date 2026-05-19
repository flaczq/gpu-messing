#include "../components/component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../graphics/renderer.h"
#include "../utils/enum_utils.hpp"
#include "game_entity.h"
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

GameEntity::GameEntity(const std::string& name, GroupID groupID)
	: m_name(name),
	  m_groupID(groupID)
{
	LOG_D("GameEntity: " << name << " with groupID: " << Utils::getEnumName(groupID) << " created!");
}

void GameEntity::init() {
	for (auto& c : m_components) {
		c->onInit();
	}
}

void GameEntity::fixedUpdate(float fixedt) const {
	for (auto& c : m_components) {
		c->onFixedUpdate(fixedt);
	}
}

void GameEntity::update(float alpha) const {
	for (auto& c : m_components) {
		c->onUpdate(alpha);
	}
}

void GameEntity::end() {
	for (auto& c : m_components) {
		c->onEnd();
	}
}