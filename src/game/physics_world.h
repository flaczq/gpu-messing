#pragma once

#include <vector>

class GameEntity;

class PhysicsWorld {
public:
	PhysicsWorld();

	void init();

private:
	std::vector<GameEntity*> m_gameEntities;
};