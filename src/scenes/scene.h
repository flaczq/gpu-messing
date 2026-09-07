#pragma once

#include "../ecs/entity.hpp"
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

	virtual bool init();
	virtual void processInput();
	virtual void saveState();
	virtual void fixedUpdate(float fixedt);
	virtual void update(float alpha);
	virtual void lateUpdate();
	virtual void end();

	virtual SceneID getID() const = 0;
	std::vector<Entity*> getGameEntites() const { return m_aliveGameEntities; }

protected:
	//std::vector<std::unique_ptr<Entity>> m_gameEntities;
	std::vector<Entity*> m_aliveGameEntities;
	std::vector<Entity*> m_deadGameEntities;
};