#include "../../configs/math_config.hpp"
#include "../../managers/input_manager.h"
#include "../../utils/component_utils.hpp"
#include "../../utils/math_utils.hpp"
#include "../components/player_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "player_system.h"

PlayerSystem::PlayerSystem() = default;

// continuous key clicks -> movement
void PlayerSystem::processInput(Registry& registry) {
    // TODO only primary Player for now
    for (Entity entity : registry.view<PlayerComponent>()) {
        auto* player = registry.getComponent<PlayerComponent>(entity);

        if (player->isPrimary) {
            // CROUCHING/STANDING
            if (InputManager::getInstance().isKeyPressed(GLFW_KEY_C)) {
                _toggleCrouching(player);
            }
            // GOD MODE
            if (InputManager::getInstance().isKeyPressed(GLFW_KEY_G)) {
                _toggleGodMode(player);
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
            if (player->isGodMode) {
                if (InputManager::getInstance().isKeyDown(GLFW_KEY_Q)) {
                    moveDir.y -= 1.0f;
                }
                if (InputManager::getInstance().isKeyDown(GLFW_KEY_E)) {
                    moveDir.y += 1.0f;
                }
            }
            // normalize diagonal movement
            player->moveDir = glm::length(moveDir) > 0.0f ? glm::normalize(moveDir) : glm::vec3(0.0f);
            break;
        }
    }
}

void PlayerSystem::fixedUpdate(Registry& registry, float fixedt) {
    // TODO only primary Player for now
    for (Entity entity : registry.view<TransformComponent, PlayerComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* player = registry.getComponent<PlayerComponent>(entity);

        if (player->isPrimary) {
            if (glm::length(player->moveDir) > 0.0f) {
                glm::vec3 up = Constants::Stats::World::WORLD_UP;
                glm::vec3 flatFront = Utils::Component::calculateFlatFront(*transform);
                glm::vec3 right = Utils::Component::calculateRight(*transform);
                glm::vec3 direction =
                    // front-back
                    flatFront * player->moveDir.z +
                    // left-right
                    right * player->moveDir.x +
                    // up-down
                    up * player->moveDir.y;
                float velocity = Constants::Stats::Player::MOVEMENT_SPEED * fixedt;
                transform->position += direction * velocity;
            }
            if (!player->isGodMode) {
                transform->position.y = 0.0f;
            }
            break;
        }
    }
}

void PlayerSystem::_toggleCrouching(PlayerComponent* player) {
    player->isCrouching = !player->isCrouching;
    LOG_D("Changed Player's crouching to: " << std::boolalpha << player->isCrouching);
}

void PlayerSystem::_toggleGodMode(PlayerComponent* player) {
    player->isGodMode = !player->isGodMode;
    LOG_D("Changed Player's GOD mode to: " << std::boolalpha << player->isGodMode);
}