#pragma once

#include "../game/game_entity.h"
#include <memory>
#include <utility>
#include <vector>

enum class SceneID {
	SOLDIER,
	RTX
	//FPS_GAME
};

class Scene {
public:
	virtual ~Scene();

	virtual void init();
	virtual void saveState();
	virtual void processInput();
	virtual void fixedUpdate(float fixedt);
	virtual void update(float alpha);
	virtual void lateUpdate();
	virtual void end();

	virtual SceneID getID() const = 0;
	std::vector<GameEntity*> getGameEntites() const { return m_aliveGameEntities; }

protected:
	std::vector<std::unique_ptr<GameEntity>> m_gameEntities;
	std::vector<GameEntity*> m_aliveGameEntities;
	std::vector<GameEntity*> m_deadGameEntities;
};