#include "../components/i_component.hpp"
#include "../configs/log_config.hpp"
#include "entity.hpp"
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
	// joke
	Entity nTityID = m_nextEntityID++;
	m_entitiesData[nTityID] = EntityData{};
	return nTityID;
}