#pragma once

#include "scene.h"

class Camera;

class RtxScene : public Scene {
public:
	RtxScene(Camera* camera);

	void init() override;
	void saveState() override;
	void fixedUpdate(float fixedt) override;
	void update(float alpha) override;
	void lateUpdate() override;
	void end() override;

	SceneID getID() const override { return SceneID::RTX; }
private:
	Camera* m_camera = nullptr;
};