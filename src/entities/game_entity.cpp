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

void GameEntity::render(float alpha) {
	m_render->draw(alpha);
}