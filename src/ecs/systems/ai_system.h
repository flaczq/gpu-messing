#pragma once

class Registry;
class Entity;

class AISystem {
public:
	AISystem();

	void onFixedUpdate(Registry& registry, float fixedt);
};