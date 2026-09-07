#include "../components/component.h"
#include "../components/component_id.hpp"
#include "entity.hpp"
#include "registry.h"
#include <memory>
#include <unordered_map>

bool Registry::init() {
	m_entitiesData.clear();
	// 0 = NULL_ENTITY
	m_nextEntityID = 1;

	return true;
}

Entity Registry::createEntity() {
	// joke
	Entity nTityID = m_nextEntityID++;
	m_entitiesData[nTityID] = EntityData{};
	return nTityID;
}