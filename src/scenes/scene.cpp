#include "../ecs/components/identity_component.hpp"
#include "../ecs/entites/entity.hpp"
#include "../ecs/registry.h"
#include "../utils/enum_utils.hpp"
#include "scene.h"

Scene::~Scene() = default;

bool Scene::init(Registry& registry) {
    for (auto& entity : registry.viewAll()) {
        auto* identity = registry.getComponent<IdentityComponent>(entity);

        LOG_D("Created entity: " << identity->name << " with ID: " << entity << " and layerID: " << Utils::Enum::getName(identity->layerID));
    }

    return true;
}

void Scene::end(Registry& registry) {
    registry.end();
}