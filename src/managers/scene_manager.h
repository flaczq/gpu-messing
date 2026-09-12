#pragma once

#include <memory>

class Registry;
class Scene;

class SceneManager {
public:
	// Meyer's Singleton
	static SceneManager& getInstance();
	SceneManager(const SceneManager&) = delete;
	void operator=(const SceneManager&) = delete;

	bool init(Registry& registry);
	void toggleScene();

private:
	// hidden constructor
	SceneManager();

	Registry& m_registry{};
	std::unique_ptr<Scene> m_currentScene{};
};