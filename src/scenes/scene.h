#pragma once

#include "../game/entities/game_object.h"
#include <memory>
#include <vector>

enum class SceneID {
	SOLDIER,
	LIGHTS_ROOM,
	FPS_GAME
};

class Scene {
public:
	virtual ~Scene() = default;

	virtual void init() = 0;
	virtual void fixedUpdate(float fixedt) = 0;
	virtual void renderFrame(float alpha) = 0;
	virtual void end() = 0;

	SceneID getSceneID() const { return m_sceneID; }
	std::vector<std::unique_ptr<GameObject>> getGameObjects() const { return m_gameObjects; }

private:
	SceneID m_sceneID{};
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;
};