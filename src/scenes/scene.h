#pragma once

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
	virtual void render(float alpha) = 0;
	virtual void end() = 0;

	SceneID getSceneID() const;

private:
	SceneID m_sceneID{};
};