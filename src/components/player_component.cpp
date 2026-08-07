#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/game_entity.h"
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
}

void PlayerComponent::processInput() {

}

void PlayerComponent::onFixedUpdate(float fixedt) {
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