#include "../game/game_entity.h"
#include "../game/physics_world.h"
#include "../graphics/renderer.h"
#include "scene.h"
#include <memory>
#include <utility>
#include <vector>

Scene::~Scene() = default;

void Scene::init() {
    // FIXME hardcoded max: 100
    m_aliveGameEntities.reserve(100);
    m_deadGameEntities.reserve(100);

    //bool isStencilReqd = false;
    //bool isOutlineReqd = false;
    //bool isBlendingReqd = false;
    for (auto& gameEntity : m_gameEntities) {
        if (gameEntity->isAlive() && !gameEntity->isPendingDeath()) {
            m_aliveGameEntities.push_back(gameEntity.get());

            //if (gameEntity->getRendererQueueType() == RendererQueueType::STENCIL) {
            //    isStencilReqd = true;
            //}
            //if (gameEntity->getRendererQueueType() == RendererQueueType::OUTLINE) {
            //    isOutlineReqd = true;
            //}
            //if (gameEntity->getRendererQueueType() == RendererQueueType::BLENDING) {
            //    isBlendingReqd = true;
            //}
        } else {
            m_deadGameEntities.push_back(gameEntity.get());
        }
    }

    // first frame Renderer params
    // reqd if i need it at the very start of a frame before update()
    //Renderer::getInstance().setStencilReqd(isStencilReqd || isOutlineReqd);
    //Renderer::getInstance().setBlendingReqd(isBlendingReqd);
}

void Scene::saveState() {
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        aliveGameEntity->getTransform()->saveState();
    }
}

void Scene::fixedUpdate(float fixedt) {
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        aliveGameEntity->fixedUpdate(fixedt);

        //if (aliveGameEntity->getPhysics()) {
        //    LOG_D(aliveGameEntity->getName() << " MIN: " << Utils::getVec3Values(aliveGameEntity->getPhysics()->getAABB().worldMin)
        //                                     << " MAX: " << Utils::getVec3Values(aliveGameEntity->getPhysics()->getAABB().worldMax));
        //}
    }
}

void Scene::update(float alpha) {
    //bool isStencilReqd = false;
    //bool isOutlineReqd = false;
    //bool isBlendingReqd = false;
    for (auto& aliveGameEntity : m_aliveGameEntities) {
        aliveGameEntity->update(alpha);

        //if (queueType == RendererQueueType::STENCIL) {
        //    isStencilReqd = true;
        //}
        //if (queueType == RendererQueueType::OUTLINE) {
        //    isOutlineReqd = true;
        //}
        //if (queueType == RendererQueueType::BLENDING) {
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
            GameEntity* gameEntity = m_aliveGameEntities[i];
            gameEntity->setPendingDeath(false);
            gameEntity->setAlive(false);
            m_deadGameEntities.push_back(gameEntity);

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

    for (auto& gameEntity : m_gameEntities) {
        gameEntity->end();
    }

    m_deadGameEntities.clear();
    m_aliveGameEntities.clear();
    m_gameEntities.clear();
}