#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct DirLightMovementComponent : public IComponent {
	glm::vec3 direction{};
	glm::vec3 color{};

	DirLightMovementComponent(glm::vec3 direction_,
							  glm::vec3 color_)
		: direction(direction_),
		  color(color_) {}
};