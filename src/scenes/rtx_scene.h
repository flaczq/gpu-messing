#pragma once

#include "scene.h"

class Registry;

class RtxScene : public Scene {
public:
	RtxScene(Registry& registry);

	bool init() override;

	SceneID getID() const override { return SceneID::RTX; }

private:
	//private
};