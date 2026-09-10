#include "../../configs/math_config.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "transform_system.h"

TransformSystem::TransformSystem() = default;

void TransformSystem::saveState(Registry& registry) {
	for (Entity entity : registry.view<TransformComponent>()) {
		auto* transform = registry.getComponent<TransformComponent>(entity);

		transform->prevPosition = transform->position;
		transform->prevRotation = transform->rotation;
		transform->prevScale = transform->scale;
	}
}

void TransformSystem::updateRotation(Registry& registry) {
	for (Entity entity : registry.view<TransformComponent>()) {
		auto* transform = registry.getComponent<TransformComponent>(entity);

		glm::quat qYaw = glm::angleAxis(glm::radians(-transform->yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qPitch = glm::angleAxis(glm::radians(transform->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		transform->rotation = qYaw * qPitch;
		//transform->prevRotation = transform->rotation;
	}
}