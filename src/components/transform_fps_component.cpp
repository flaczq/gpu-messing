#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "transform_component.h"
#include "transform_fps_component.h"

TransformFpsComponent::TransformFpsComponent(Camera* camera)
    : TransformComponent(glm::vec3(0.0f)),
      m_camera(camera)
{
}

void TransformFpsComponent::saveState() {
    TransformComponent::saveState();
    m_prevSway = m_sway;
}

void TransformFpsComponent::onFixedUpdate(float fixedt) {
    m_sway += fixedt * 2.0f;
    m_position = glm::vec3(m_model[3]);
    m_dirty = true;
}

glm::mat4 TransformFpsComponent::getInterpolatedModelMatrix(float alpha) {
    // make me sway
    float interSway = glm::mix(m_prevSway, m_sway, alpha);
    float swayX = cos(interSway * 0.5f) * 0.3f;
    float swayZ = sin(interSway * 1.0f) * 0.2f;
    glm::vec3 interPosition = glm::mix(m_camera->getPreViewPos(), m_camera->getViewPos(), alpha);
    glm::mat3 viewRotation = glm::mat3(m_camera->getViewMatrix());
    m_model = glm::translate(glm::mat4(1.0f), interPosition);
    m_model *= glm::mat4(glm::transpose(viewRotation));
    // rotation fix
    m_model = glm::rotate(m_model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // scale fix
    m_model = glm::scale(m_model, glm::vec3(0.2f));
    // local offset
    glm::vec3 offset = glm::vec3(10.0f, 0.0f, 2.0f) + glm::vec3(swayX, 0.0f, swayZ);
    m_model = glm::translate(m_model, offset);
    return m_model;
}