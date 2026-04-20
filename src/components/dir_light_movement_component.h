#pragma once

#include "component.h"

class Renderer;
class TransformComponent;

class DirLightMovementComponent : public Component {
public:
	DirLightMovementComponent(Renderer* renderer);

	void onInit() override;
	void onFixedUpdate(float fixedt) override;

private:
	TransformComponent* m_transform = nullptr;
	Renderer* m_renderer = nullptr;
};