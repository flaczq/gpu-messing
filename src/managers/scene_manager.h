#pragma once

#include <memory>

class Registry;
class PhysicsSystem;
class Camera;
class Scene;

class SceneManager {
public:
	// Meyer's Singleton
	static SceneManager& getInstance();
	SceneManager(const SceneManager&) = delete;
	void operator=(const SceneManager&) = delete;

	bool init(Registry& registry, PhysicsSystem& physicsSystem, Camera* camera);
	void toggleScene();
	void processInput();
	void saveState() const;
	void fixedUpdate(float fixedt) const;
	void update(float alpha) const;
	void lateUpdate() const;
	void end() const;

private:
	// hidden constructor
	SceneManager();

	Registry& m_registry;
	PhysicsSystem& m_physicsSystem;

	Camera* m_camera = nullptr;

	std::unique_ptr<Scene> m_currentScene;
};