#pragma once

#include <memory>

class Scene;
class Camera;

class SceneManager {
public:
	SceneManager(Camera* camera);
	~SceneManager();

	bool init();
	void toggleScene();
	void fixedUpdate(float fixedt) const;
	void renderFrame(float alpha) const;

private:
	Camera* m_camera = nullptr;
	std::unique_ptr<Scene> m_currentScene;
};