#include "../components/component.h"
#include "../components/components_types.hpp"
#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "game_entity.h"
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

GameEntity::GameEntity(const std::string& name)
	: m_name(name)
{
	LOG_D("GameEntity: " << name << " created!");
}

// TODO delete this, instead create entities queue
bool GameEntity::checkStatus() const {
	if (!m_alive) {
		LOG_D("GameEntity: " << m_name << " -> not alive!");
		return false;
	}
	if (!m_transform) {
		LOG_D("GameEntity: " << m_name << " -> no TransformComponent!");
		return false;
	}
	if (!m_render) {
		LOG_D("GameEntity: " << m_name << " -> no RenderComponent!");
		return false;
	}

	return true;
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