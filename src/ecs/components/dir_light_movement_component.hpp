#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct DirLightMovementComponent : public IComponent {
	glm::vec3 direction{};
	glm::vec3 color{};
	bool isPrimary{};

	DirLightMovementComponent(glm::vec3 direction_,
							  glm::vec3 color_,
							  bool isPrimary_ = true)
		: direction(direction_),
		  color(color_),
		  isPrimary(isPrimary_) {}
};