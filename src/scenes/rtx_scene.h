#pragma once

#include "scene.h"

class Registry;

class RtxScene : public Scene {
public:
	RtxScene();

	bool init(Registry& registry) override;
	void end(Registry& registry) override;
	SceneID getID() const override { return SceneID::RTX; }

//private:
};