#pragma once

class Registry;

class TransformSystem {
public:
	TransformSystem();

	void saveState(Registry& registry);
	void updateRotation(Registry& registry);
};