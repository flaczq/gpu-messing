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
		//if constexpr (std::is_base_of_v<TransformComponent, T>) {
		//	m_transform = static_cast<TransformComponent*>(cPtr);
		//} else if constexpr (std::is_base_of_v<RenderComponent, T>) {
		//	m_render = static_cast<RenderComponent*>(cPtr);
		//} else if constexpr (std::is_base_of_v<PhysicsComponent, T>) {
		//	m_physics = static_cast<PhysicsComponent*>(cPtr);
		//} else if constexpr (std::is_base_of_v<PlayerComponent, T>) {
		//	m_player = static_cast<PlayerComponent*>(cPtr);
		//}
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

	//void fixedUpdate(float fixedt) const;
	//void update(float alpha) const;

	//const std::string& getName() const { return m_name; }
	//GroupID getGroupID() const { return m_groupID; }
	//TransformComponent* getTransform() { return m_transform; }
	//RenderComponent* getRender() { return m_render; }
	//PhysicsComponent* getPhysics() { return m_physics; }
	//PlayerComponent* getPlayer() { return m_player; }
	//RendererQueueType getRendererQueueType() const { return m_rendererQueueType; }
	//void setRendererQueueType(RendererQueueType rendererQueueType) { m_rendererQueueType = rendererQueueType; }
	//bool isSolid() const { return m_solid; }
	//void setSolid(bool solid) { m_solid = solid; }
	//bool isAbstract() const { return m_abstract; }
	//void setAbstract(bool abstract) { m_abstract = abstract; }
	//bool isAlive() const { return m_alive; }
	//void setAlive(bool alive) { m_alive = alive; }
	//bool isPendingDeath() const { return m_pendingDeath; }
	//void setPendingDeath(bool pendingDeath) { m_pendingDeath = pendingDeath; }
	//void destroy() { m_pendingDeath = true; }
private:
	struct EntityData {
		std::unordered_map<ComponentTypeID, std::unique_ptr<IComponent>> components;
	};

	Entity m_nextEntityID = 1;
	std::unordered_map<Entity, EntityData> m_entitiesData;

	//TransformComponent* m_transform = nullptr;
	//RenderComponent* m_render = nullptr;
	//PhysicsComponent* m_physics = nullptr;
	//PlayerComponent* m_player = nullptr;
	//RendererQueueType m_rendererQueueType = RendererQueueType::OPAQUE;
	//// no collisions e.g. grass, ...
	//bool m_solid = false;
	//// unaffected by physics e.g. light, trigger, ...
	//bool m_abstract = false;
	//bool m_alive = true;
	//bool m_pendingDeath = false;
};