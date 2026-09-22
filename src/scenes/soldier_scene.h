#pragma once

#include "scene.h"

class Registry;

class SoldierScene : public Scene {
public:
	SoldierScene();

	bool init(Registry& registry) override;
	void end(Registry& registry) override;
	SceneID getID() const override { return SceneID::SOLDIER; }

//private:
};