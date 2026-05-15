#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/game_entity.h"
#include "component.h"
#include "transform_fps_component.h"

TransformFpsComponent::TransformFpsComponent(Camera* camera)
    : m_camera(camera)
{
}

void TransformFpsComponent::onInit() {
    m_transform = getOwner()->getTransform();
}

glm::mat4 TransformFpsComponent::getInterpolatedModelMatrix(float alpha) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, m_camera->getViewPos());
    model *= glm::mat4(glm::transpose(glm::mat3(m_camera->getViewMatrix())));
    // rotation fix
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // scale fix
    model = glm::scale(model, glm::vec3(0.2f));
    // local offset
    model = glm::translate(model, glm::vec3(9.0f, 0.0f, 2.0f));
    return model;
}