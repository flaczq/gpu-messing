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

	bool init(Registry& registry, PhysicsSystem& physicsSystem, Camera& camera);
	void toggleScene();

private:
	// hidden constructor
	SceneManager();

	Registry& m_registry;
	PhysicsSystem& m_physicsSystem;
	Camera& m_camera;

	std::unique_ptr<Scene> m_currentScene;
};