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
	void toggleScene(Registry& registry);

private:
	// hidden constructor
	SceneManager();

	std::unique_ptr<Scene> m_currentScene{};
};