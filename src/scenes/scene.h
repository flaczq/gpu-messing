#pragma once

#include "../entities/game_entity.h"
#include <memory>
#include <utility>
#include <vector>

enum class SceneID {
	SOLDIER,
	LIGHTS_ROOM,
	FPS_GAME
};

class Scene {
public:
	virtual ~Scene();

	virtual void init() = 0;
	virtual void saveState() = 0;
	virtual void fixedUpdate(float fixedt) = 0;
	virtual void renderFrame(float alpha) = 0;
	virtual void end() = 0;

	virtual SceneID getID() const = 0;

protected:
	std::vector<std::unique_ptr<GameEntity>> m_gameEntities;
};