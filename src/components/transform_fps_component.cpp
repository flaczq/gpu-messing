#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/game_entity.h"
#include "transform_component.h"
#include "transform_fps_component.h"

TransformFpsComponent::TransformFpsComponent(Camera* camera)
    : TransformComponent(glm::vec3(0.0f)),
      m_camera(camera)
{
}

glm::mat4 TransformFpsComponent::getInterpolatedModelMatrix(float alpha) {
    // TODO interpolation
    m_model = glm::translate(glm::mat4(1.0f), m_camera->getViewPos());
    m_model *= glm::mat4(glm::transpose(glm::mat3(m_camera->getViewMatrix())));
    // rotation fix
    m_model = glm::rotate(m_model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // scale fix
    m_model = glm::scale(m_model, glm::vec3(0.2f));
    // local offset
    m_model = glm::translate(m_model, glm::vec3(9.0f, 0.0f, 2.0f));
    return m_model;
}