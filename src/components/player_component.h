#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

enum class VerticalMode {
	STANDING,
	CROUCHING
};

class Camera;
class TransformComponent;

class PlayerComponent : public Component {
public:
	PlayerComponent(Camera* camera);

	void onInit() override;
	void processInput();
	void onFixedUpdate(float fixedt) override;

	void toggleVerticalMode();
	void toggleGodMode();
	bool isCrouching() const;

private:
	static constexpr float MOVEMENT_SPEED = 5.0f;

	Camera* m_camera = nullptr;
	TransformComponent* m_transform = nullptr;
	
	int m_health{};
	VerticalMode m_verticalMode{};

	glm::vec3 m_moveDir = glm::vec3(0.0f);
	bool m_verticalModeChanged = false;
	bool m_godMode = false;
	bool m_godModeChanged = false;
};