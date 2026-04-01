#include "../components/component.h"
#include "../components/components_types.hpp"
#include "../components/transform_component.h"
#include "../configs/math_config.hpp"
#include "game_entity.h"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T, typename... TArgs>
T& GameEntity::addComponent(TArgs&&... args) {
	auto comp = std::make_unique<T>(std::forward<TArgs>(args)...);
	T* compPtr = comp.get();
	compPtr->m_owner = this;
	//unsigned int id = ComponentType<T>::getID();
	//m_componentsMap[id] = c;

	if constexpr (std::is_base_of_v<TransformComponent, T>) {
		m_transform = static_cast<TransformComponent*>(compPtr);
	} else if constexpr (std::is_base_of_v<TransformComponent, T>) {
		m_renderer = static_cast<TransformComponent*>(compPtr);
	}

	m_components.push_back(std::move(comp));
	return *compPtr;
}

void GameEntity::render(float alpha) {
	m_renderer->getInterpolatedMatrix(alpha);
}