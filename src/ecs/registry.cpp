#include "../configs/log_config.hpp"
#include "./components/i_component.hpp"
#include "./entites/entity.hpp"
#include "registry.h"
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

bool Registry::init() {
	m_entitiesData.clear();
	// 0 = NULL_ENTITY
	m_nextEntityID = 1;

	return true;
}

Entity Registry::createEntity() {
	// inside joke
	Entity nTityID = m_nextEntityID++;
	m_entitiesData[nTityID] = EntityData{};
	return nTityID;
}

std::vector<Entity> Registry::viewAll() const {
	std::vector<Entity> allEntities;
	allEntities.reserve(m_entitiesData.size());
	for (const auto& [entity, data] : m_entitiesData) {
		allEntities.push_back(entity);
	}
	return allEntities;
}

void Registry::end() {
	m_entitiesData.clear();
}