#pragma once

#include "../components/component.h"
#include "../components/components_types.hpp"
#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../configs/math_config.hpp"
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

class GameEntity {
public:
	GameEntity(const std::string& name = "Entity");

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... args) {
		auto comp = std::make_unique<T>(std::forward<TArgs>(args)...);
		T* compPtr = comp.get();
		compPtr->m_owner = this;
		//unsigned int id = ComponentType<T>::getID();
		//m_componentsMap[id] = c;

		if constexpr (std::is_base_of_v<TransformComponent, T>) {
			m_transform = static_cast<TransformComponent*>(compPtr);
		} else if constexpr (std::is_base_of_v<RenderComponent, T>) {
			m_render = static_cast<RenderComponent*>(compPtr);
		}

		m_components.push_back(std::move(comp));
		return *compPtr;
	}
	
	const std::string& getName() const { return m_name; }
	TransformComponent* getTransform() { return m_transform; }
	RenderComponent* getRender() { return m_render; }

private:
	std::string m_name;
	std::vector<std::unique_ptr<Component>> m_components;
	TransformComponent* m_transform = nullptr;
	RenderComponent* m_render = nullptr;
};