#pragma once

#include "scene.h"

class Registry;
class PhysicsSystem;

class SoldierScene : public Scene {
public:
	SoldierScene(Registry& registry, PhysicsSystem& physicsSystem);

	bool init() override;
	void saveState() override;
	void fixedUpdate(float fixedt) override;
	void update(float alpha) override;
	void lateUpdate() override;
	void end() override;

	SceneID getID() const override { return SceneID::SOLDIER; }

private:
	//private
};