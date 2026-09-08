#pragma once

class Registry;

class PhysicsSystem {
public:
	PhysicsSystem();

	void fixedUpdate(Registry& registry, float fixedt);
};