#pragma once

class Registry;

class DirLightMovementSystem {
public:
	DirLightMovementSystem();

	void fixedUpdate(Registry& registry, float fixedt);
};