#include "../ecs/entites/entity.hpp"
#include "../ecs/registry.h"
#include "scene.h"

Scene::Scene(Registry& registry, PhysicsSystem& physicsSystem)
    : m_registry(registry),
      m_physicsSystem(physicsSystem)
{
}

Scene::~Scene() = default;

bool Scene::init() {
    for (auto& entity : m_registry.viewAll()) {
        LOG_D(entity);
    }

    // first frame Renderer params
    // reqd if i need it at the very start of a frame before update()
    //Renderer::getInstance().setStencilReqd(isStencilReqd || isOutlineReqd);
    //Renderer::getInstance().setBlendingReqd(isBlendingReqd);

    return true;
}

void Scene::processInput() {
    for (auto& aliveEntity : m_aliveGameEntities) {
        if (aliveEntity->getPlayer()) {
            aliveEntity->getPlayer()->processInput();
        }
    }
}

void Scene::saveState() {
    for (auto& aliveEntity : m_aliveGameEntities) {
        if (aliveEntity->getTransform()) {
            aliveEntity->getTransform()->saveState();
        }
    }
}

void Scene::fixedUpdate(float fixedt) {
    aiSystem.fixedUpdate(m_registry, fixedt);
}

void Scene::update(float alpha) {
    //bool isStencilReqd = false;
    //bool isOutlineReqd = false;
    //bool isBlendingReqd = false;
    for (auto& aliveEntity : m_aliveGameEntities) {
        aliveEntity->update(alpha);

        //if (queueType == RenderQueueType::STENCIL) {
        //    isStencilReqd = true;
        //}
        //if (queueType == RenderQueueType::OUTLINE) {
        //    isOutlineReqd = true;
        //}
        //if (queueType == RenderQueueType::BLENDING) {
        //    isBlendingReqd = true;
        //}
    }

    // next frame Renderer param
    // not reqd because i can check if queue is empty()
    //Renderer::getInstance().setStencilReqd(isStencilReqd || isOutlineReqd);
    //Renderer::getInstance().setBlendingReqd(isBlendingReqd);
}

// renderrring
void Scene::lateUpdate() {
    for (int i = 0; i < m_aliveGameEntities.size(); /*i++*/) {
        //LOG_D("CHECKING " << m_aliveGameEntities[i]->getName());
        if (m_aliveGameEntities[i]->isPendingDeath()) {
            //LOG_D("DEAD " << m_aliveGameEntities[i]->getName());
            Entity* Entity = m_aliveGameEntities[i];
            Entity->setPendingDeath(false);
            Entity->setAlive(false);
            m_deadGameEntities.push_back(Entity);

            // fast delete (swap & pop)
            m_aliveGameEntities[i] = m_aliveGameEntities.back();
            m_aliveGameEntities.pop_back();
            // 'i' -> new element from back
        } else {
            i++;
        }
    }
    //LOG_D("ALL " << m_gameEntities.size());
    //LOG_D("ALIVE " << m_aliveGameEntities.size());
    //LOG_D("DEAD " << m_deadGameEntities.size());
}

void Scene::end() {
    PhysicsWorld::getInstance().end();

    m_deadGameEntities.clear();
    m_aliveGameEntities.clear();
    m_gameEntities.clear();
}