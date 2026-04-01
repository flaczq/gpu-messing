#include "../components/component.h"
#include "../components/components_types.hpp"
#include "../components/transform_component.h"
#include "../configs/math_config.hpp"
#include "game_entity.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

void GameEntity::render(float alpha) {
	m_renderer->getInterpolatedMatrix(alpha);
}