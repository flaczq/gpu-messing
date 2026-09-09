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

	virtual bool init();
	virtual void processInput();
	virtual void saveState();
	virtual void fixedUpdate(float fixedt);
	virtual void update(float alpha);
	virtual void lateUpdate();
	virtual void end();

	virtual SceneID getID() const = 0;

protected:
	Registry& m_registry;
	// systems for all scenes
	//PhysicsSystem& m_physicsSystem;
};