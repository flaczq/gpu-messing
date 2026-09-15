#include "../ecs/components/identity_component.hpp"
#include "../ecs/entites/entity.hpp"
#include "../ecs/registry.h"
#include "scene.h"

Scene::~Scene() = default;

bool Scene::init(Registry& registry) {
    for (auto& entity : registry.viewAll()) {
        auto* identity = registry.getComponent<IdentityComponent>(entity);

        LOG_D("Created entity: " << identity->name << " with ID: " << entity);
    }

    return true;
}

void Scene::end(Registry& registry) {
    registry.end();
}