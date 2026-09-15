#pragma once

class Registry;

class AISystem {
public:
	AISystem();

	void fixedUpdate(Registry& registry, float fixedt);
};