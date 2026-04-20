#pragma once

#include <memory>

class Camera;
class Renderer;
class Scene;

class SceneManager {
public:
	SceneManager(Camera* camera, Renderer* renderer);
	~SceneManager();

	bool init();
	void toggleScene();
	void saveState() const;
	void fixedUpdate(float fixedt) const;
	void update(float alpha) const;

private:
	Camera* m_camera = nullptr;
	Renderer* m_renderer = nullptr;

	std::unique_ptr<Scene> m_currentScene;
};