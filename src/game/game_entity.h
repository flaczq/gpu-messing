#pragma once

#include "../components/component.h"
#include "../components/components_types.hpp"
#include "../components/render_component.h"
#include "../components/transform_component.h"
#include "../configs/math_config.hpp"
#include "../graphics/renderer.h"
#include <iostream>
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
		auto c = std::make_unique<T>(std::forward<TArgs>(args)...);
		T* cPtr = c.get();
		cPtr->m_owner = this;
		//unsigned int id = ComponentCounter<T>::getTypeID();
		//m_componentsMap[id] = c;

		if constexpr (std::is_base_of_v<TransformComponent, T>) {
			m_transform = static_cast<TransformComponent*>(cPtr);
		} else if constexpr (std::is_base_of_v<RenderComponent, T>) {
			m_render = static_cast<RenderComponent*>(cPtr);
		}

		m_components.push_back(std::move(c));
		return *cPtr;
	}

	void init();
	void fixedUpdate(float fixedt) const;
	void update(float alpha) const;
	void end();

	const std::string& getName() const { return m_name; }
	TransformComponent* getTransform() { return m_transform; }
	RenderComponent* getRender() { return m_render; }
	RendererQueueType getRendererQueueType() const { return m_rendererQueueType; }

	void setRendererQueueType(RendererQueueType rendererQueueType) { m_rendererQueueType = rendererQueueType; }
	bool isSolid() const { return m_solid; }
	void setSolid(bool solid) { m_solid = solid; }
	bool isAbstract() const { return m_abstract; }
	void setAbstract(bool abstract) { m_abstract = abstract; }
	bool isAlive() const { return m_alive; }
	void setAlive(bool alive) { m_alive = alive; }
	bool isPendingDeath() const { return m_pendingDeath; }
	void setPendingDeath(bool pendingDeath) { m_pendingDeath = pendingDeath; }
	void destroy() { m_pendingDeath = true; }

private:
	std::string m_name;
	std::vector<std::unique_ptr<Component>> m_components;

	TransformComponent* m_transform = nullptr;
	RenderComponent* m_render = nullptr;
	RendererQueueType m_rendererQueueType = RendererQueueType::FIRST;
	// no gravity
	bool m_solid = false;
	// "invisible" e.g. light, trigger, ...
	bool m_abstract = false;
	bool m_alive = true;
	bool m_pendingDeath = false;
};