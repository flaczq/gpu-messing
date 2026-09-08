#pragma once

#include "scene.h"

class Registry;

class RtxScene : public Scene {
public:
	RtxScene(Registry& registry);

	bool init() override;
	void saveState() override;
	void fixedUpdate(float fixedt) override;
	void update(float alpha) override;
	void lateUpdate() override;
	void end() override;

	SceneID getID() const override { return SceneID::RTX; }

private:
	//private
};