#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class Camera;
class TransformComponent;

enum class VerticalMode {
	STANDING,
	CROUCHING
};

class PlayerComponent : public Component {
public:
	PlayerComponent(Camera* camera);

	void onInit() override;
	void processInput();
	void onFixedUpdate(float fixedt) override;

	void toggleVerticalMode();
	void toggleGodMode();

private:
	TransformComponent* m_transform = nullptr;
	Camera* m_camera = nullptr;

	int m_health{};
	glm::vec3 m_position{}, m_prevPosition{};
	glm::quat m_rotation{}, m_prevRotation{};
	glm::vec3 m_scale{}, m_prevScale{};

	VerticalMode m_verticalMode = VerticalMode::STANDING;
	bool m_verticalModeChanged = false;
	bool m_godMode = false;
	bool m_godModeChanged = false;
};