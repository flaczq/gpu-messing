#pragma once

#include <memory>

class Camera;
class Scene;

class SceneManager {
public:
	// Meyer's Singleton
	static SceneManager& getInstance();
	SceneManager(const SceneManager&) = delete;
	void operator=(const SceneManager&) = delete;

	bool init(Camera* camera);
	void toggleScene();
	void saveState() const;
	void fixedUpdate(float fixedt) const;
	void update(float alpha) const;
	void lateUpdate() const;
	void end() const;

private:
	// hidden constructor
	SceneManager();

	Camera* m_camera = nullptr;

	std::unique_ptr<Scene> m_currentScene;
};