#pragma once

enum class SceneID {
	SOLDIER,
	RTX
	//FPS_GAME
};

class Registry;

class Scene {
public:
	virtual ~Scene();

	virtual bool init(Registry& registry) = 0;
	virtual void end(Registry& registry) = 0;
	virtual SceneID getID() const = 0;

//protected:
};