#include "ai_component.h"
#include "component.h"

//void AIComponent::onFixedUpdate(float fixedt) {
//    if (!getOwner()->getPhysics()->isColliding()) {
//        static float tt = 0.0f;
//        tt += fixedt;
//        float x = sin(tt * 0.5f) * 2.0f;
//        float z = cos(tt);
//        glm::vec3 newPos = glm::normalize(glm::vec3(x, 0.0f, z));
//        m_transform->setPosition(newPos);
//        m_transform->setRotation(newPos);
//    }
//}