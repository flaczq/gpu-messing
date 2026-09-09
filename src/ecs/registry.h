#pragma once

#include "../configs/log_config.hpp"
#include "./components/i_component.hpp"
#include "./entites/entity.hpp"
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

class Registry {
public:
	bool init();
	Entity createEntity();
	std::vector<Entity> viewAll() const;
	void end();

	template <typename T, typename... TArgs>
	T& addComponent(Entity entity, TArgs&&... args) {
		auto c = std::make_unique<T>(std::forward<TArgs>(args)...);
		T* cPtr = c.get();
		ComponentTypeID cTypeID = ComponentID::get<T>();
		m_entitiesData[entity].components[cTypeID] = std::move(c);
		return *cPtr;
	}
	template <typename T>
	T* getComponent(Entity entity) {
		auto it = m_entitiesData.find(entity);
		if (it == m_entitiesData.end()) {
			LOG_E("REGISTRY::GET_COMPONENT_ENTITY_NULLPTR: " << entity);
			return false;
		}

		// same every time.. i hope..?
		ComponentTypeID cTypeID = ComponentID::get<T>();
		auto cIt = it->second.components.find(cTypeID);
		if (cIt == it->second.components.end()) {
			LOG_W("REGISTRY::GET_COMPONENT_COMPONENT_NULLPTR: " << cTypeID);
			return nullptr;
		}
		return static_cast<T*>(cIt->second.get());
	}
	template <typename T>
	bool hasComponent(Entity entity) const {
		auto it = m_entitiesData.find(entity);
		if (it == m_entitiesData.end()) {
			LOG_E("REGISTRY::GET_COMPONENT_ENTITY_NULLPTR: " << entity);
			return false;
		}

		ComponentTypeID cTypeID = ComponentID::get<T>();
		auto cIt = it->second.components.find(cTypeID);
		// found
		return cIt != it->second.components.end();
	}
	// fold expression
	template <typename... Comps>
	bool hasComponents(Entity entity) const {
		return (hasComponent<Comps>(entity) && ...);
	}
	template <typename... Comps>
	std::vector<Entity> view() const {
		std::vector<Entity> matchingEntities;
		for (const auto& [entity, data] : m_entitiesData) {
			if (hasComponents<Comps...>(entity)) {
				matchingEntities.push_back(entity);
			}
		}
		return matchingEntities;
	}

private:
	struct EntityData {
		std::unordered_map<ComponentTypeID, std::unique_ptr<IComponent>> components;
	};

	Entity m_nextEntityID = 1;
	std::unordered_map<Entity, EntityData> m_entitiesData;
};