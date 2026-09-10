#pragma once

#include "scene.h"

class Registry;

class SoldierScene : public Scene {
public:
	SoldierScene(Registry& registry);

	bool init() override;

	SceneID getID() const override { return SceneID::SOLDIER; }

private:
	//private
};