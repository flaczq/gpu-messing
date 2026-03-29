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
	void update(float dt) const;
	void renderFrame() const;
private:
	Camera* m_camera = nullptr;
	std::unique_ptr<Scene> m_currentScene;
};