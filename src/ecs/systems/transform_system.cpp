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