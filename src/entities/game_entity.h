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
	T& addComponent(TArgs&&... args) {
		auto component = std::make_unique<T>(std::forward<TArgs>(args)...);
		T* componentPtr = component.get();
		componentPtr->m_owner = this;
		//unsigned int id = ComponentType<T>::getID();
		//m_componentsMap[id] = c;

		if constexpr (std::is_base_of_v<TransformComponent, T>) {
			m_transform = static_cast<TransformComponent*>(componentPtr);
		} else if constexpr (std::is_base_of_v<TransformComponent, T>) {
			m_renderer = static_cast<TransformComponent*>(componentPtr);
		}

		m_components.push_back(std::move(component));
		return *componentPtr;
	}

	void render(float alpha);
	
	TransformComponent* getTransform() { return m_transform; }

private:
	std::vector<std::unique_ptr<Component>> m_components;
	TransformComponent* m_transform = nullptr;
	TransformComponent* m_renderer = nullptr;
};