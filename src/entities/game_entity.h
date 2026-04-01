#pragma once

#include "../components/component.h"
#include "../components/components_types.hpp"
#include "../components/transform_component.h"
#include "../configs/math_config.hpp"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

class GameEntity {
public:
	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... args);

	void render(float alpha);
	
	TransformComponent* getTransform() { return m_transform; }
	TransformComponent* getRender() { return m_renderer; }

private:
	std::vector<std::unique_ptr<Component>> m_components;
	TransformComponent* m_transform = nullptr;
	TransformComponent* m_renderer = nullptr;
};