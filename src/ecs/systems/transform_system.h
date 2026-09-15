#pragma once

class Registry;

class TransformSystem {
public:
	TransformSystem();

	void saveState(Registry& registry);
};