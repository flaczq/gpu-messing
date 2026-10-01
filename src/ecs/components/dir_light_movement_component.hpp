#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct DirLightMovementComponent : public IComponent {
	glm::vec3 direction{};
	glm::vec3 color{};
	bool isPrimary{};

	DirLightMovementComponent() = default;
	DirLightMovementComponent(const glm::vec3& direction_,
							  const glm::vec3& color_,
							  bool isPrimary_)
		: direction(direction_),
		  color(color_),
		  isPrimary(isPrimary_) {}
};