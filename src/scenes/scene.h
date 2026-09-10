#pragma once

enum class SceneID {
	SOLDIER,
	RTX
	//FPS_GAME
};

class Registry;

class Scene {
public:
	// force transparency
	explicit Scene(Registry& registry);
	virtual ~Scene();

	virtual bool init() = 0;
	virtual SceneID getID() const = 0;

protected:
	Registry& m_registry;
};