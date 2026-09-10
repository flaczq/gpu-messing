#include "../../graphics/renderer.h"
#include "../components/render_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "render_system.h"

RenderSystem::RenderSystem() = default;

void RenderSystem::update(Registry& registry, float alpha) {
    for (Entity entity : registry.view<TransformComponent, RenderComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* render = registry.getComponent<RenderComponent>(entity);

        glm::mat4 modelMatrix = transform->getInterpolatedModelMatrix(alpha);
        glm::mat3 normalMatrix = transform->getNormalMatrix();
        glm::vec3 interPosition = transform->getInterpolatedPosition(alpha);

        RenderQueueType queueType = render->queueType;
        RendererCommand command = {
            render->model.get(),
            render->material.get(),
            modelMatrix,
            normalMatrix,
            interPosition
        };
        Renderer::getInstance().registerInQueue(queueType, command);
    }
}