#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../graphics/graphics_types.hpp"
#include "scene.h"
#include <memory>
#include <utility>
#include <vector>

class Camera;
class Model;
class Mesh;

class SoldierScene : public Scene {
public:
	SoldierScene(Camera* camera);

	void init() override;
	void saveState() override;
	void fixedUpdate(float fixedt) override;
	void update(float alpha) override;
	void lateUpdate() override;
	void end() override;

	SceneID getID() const override { return SceneID::SOLDIER; }

private:
	Camera* m_camera = nullptr;
};