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
	void end() override;

	SceneID getID() const override { return SceneID::SOLDIER; }

private:
	static constexpr glm::vec3 SOLDIER_POSITION = glm::vec3(3.0f, 0.0f, 3.0f);
	static constexpr float SOLDIER_ROTATION = glm::radians(-90.0f);
	static constexpr glm::vec3 SOLDIER_SCALE = glm::vec3(100.0f);
	static constexpr glm::vec3 FLOOR_POSITION = glm::vec3(10.0f, -2.0f, 10.0f);
	static constexpr float FLOOR_ROTATION = glm::radians(0.0f);
	static constexpr glm::vec3 FLOOR_SCALE = glm::vec3(1.0f);

	Camera* m_camera = nullptr;
	glm::vec3 m_lightDir = glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f));

	std::unique_ptr<Mesh> m_lightMarker;

	std::vector<Vertex> calculateLightVertices();
	std::vector<unsigned int> calculateLightIndices();
};