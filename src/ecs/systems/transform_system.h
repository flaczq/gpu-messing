#pragma once

#include "../../configs/math_config.hpp"

class Registry;

class TransformSystem {
public:
	TransformSystem();

	void saveState(Registry& registry);
	void updateRotation(Registry& registry);
	glm::mat4 getInterpolatedModelMatrix(Registry& registry, float alpha);
	glm::mat4 getNormalMatrix(Registry& registry);
	glm::vec3 getFront(Registry& registry) const;
	glm::vec3 getFlatFront(Registry& registry) const;
	glm::vec3 getRight(Registry& registry) const;
	glm::vec3 getUp(Registry& registry) const;
};