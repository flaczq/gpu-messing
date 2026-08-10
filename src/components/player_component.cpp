#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/game_entity.h"
#include "../managers/input_manager.h"
#include "../utils/enum_utils.hpp"
#include "player_component.h"
#include "transform_component.h"
#include <string>

PlayerComponent::PlayerComponent(Camera* camera)
	: m_camera(camera)
{
}

void PlayerComponent::onInit() {
	m_transform = getOwner()->getTransform();
	m_health = 100;
    m_verticalMode = VerticalMode::STANDING;

    // set the camera
    m_camera->follow(m_transform);
}

// continuous key clicks -> movement
void PlayerComponent::processInput() {
    // CROUCHING/STANDING
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_C)) {
        toggleVerticalMode();
    }
    // GOD MODE
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_G)) {
        toggleGodMode();
    }

    // MOVEMENT
    glm::vec3 moveDir = glm::vec3(0.0f);
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_W)) {
        moveDir.z += 1.0f;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_S)) {
        moveDir.z -= 1.0f;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_A)) {
        moveDir.x -= 1.0f;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_D)) {
        moveDir.x += 1.0f;
    }
    // GOD MODE ACTIVATED
    if (m_godMode) {
        if (InputManager::getInstance().isKeyDown(GLFW_KEY_Q)) {
            moveDir.y -= 1.0f;
        }
        if (InputManager::getInstance().isKeyDown(GLFW_KEY_E)) {
            moveDir.y += 1.0f;
        }
    } else {
        moveDir.y = 0.0f;
    }
    // normalize diagonal movement
    m_moveDir = glm::length(moveDir) > 0.0f ? glm::normalize(moveDir) : glm::vec3(0.0f);
}

void PlayerComponent::onFixedUpdate(float fixedt) {
    if (glm::length(m_moveDir) > 0.0f) {
        glm::vec3 flatFront = m_transform->getFlatFront();
        glm::vec3 up = m_transform->getUp();
        glm::vec3 right = m_transform->getRight();
        glm::vec3 direction = // front-back
                              (flatFront * m_moveDir.z) +
                              // left-right
                              (right * m_moveDir.x) +
                              // up-down
                              (glm::vec3(0.0f, 1.0f, 0.0f) * m_moveDir.y);
        float velocity = MOVEMENT_SPEED * fixedt;
        m_transform->addPosition(direction * velocity);
    }

    if (m_godModeChanged || m_verticalModeChanged) {
        //currPosition.y = 0.0f;
        //m_transform->setPosition(currPosition);
        m_godModeChanged = false;
        m_verticalModeChanged = false;
    }
}

void PlayerComponent::toggleVerticalMode() {
    m_verticalModeChanged = true;
    m_verticalMode = Utils::getEnumNext(m_verticalMode);
    LOG_D("Changed Player's vertical mode to: " << Utils::getEnumName(m_verticalMode));
}

void PlayerComponent::toggleGodMode() {
    m_godMode = !m_godMode;
    m_godModeChanged = true;
    LOG_D("Changed Player's GOD mode to: " << std::boolalpha << m_godMode);
}

bool PlayerComponent::isCrouching() const {
    return m_verticalMode == VerticalMode::CROUCHING;
}