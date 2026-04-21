#pragma once

#include "component.h"

class TransformComponent;

class DirLightMovementComponent : public Component {
public:
	void onInit() override;
	void onFixedUpdate(float fixedt) override;

private:
	TransformComponent* m_transform = nullptr;
};