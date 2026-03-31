#pragma once

#include "../../configs/math_config.hpp"
#include "../components/components_types.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

class Component;

class GameObject {
public:
	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... args) {
		T* c = new T(std::forward<TArgs>(args)...);
		c->owner = this;
		unsigned int id = ComponentType<T>::getID();
		m_componentsMap[id] = c;
		m_components.emplace_back(std::unique_ptr<Component>(c));
		return *c;
	}

	template <typename T>
	T* getComponent() const {
		int id = ComponentType<T>::getID();
		auto it = m_componentsMap.find(id);
		if (it != m_componentsMap.end()) {
			return static_cast<T*>(it->second);
		}
		return nullptr;
	}

private:
	std::unordered_map<int, Component*> m_componentsMap;
	std::vector<std::unique_ptr<Component>> m_components;
};