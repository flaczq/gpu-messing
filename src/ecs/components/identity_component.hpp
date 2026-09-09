#pragma once

#include "i_component.hpp"
#include <string>

enum class LayerID {
	DEFAULT,
	SOLDIERS
};

struct IdentityComponent : public IComponent {
	std::string name{};
	LayerID layerID{};

	IdentityComponent() = default;
	IdentityComponent(const std::string& name_,
					  LayerID layerID_ = LayerID::DEFAULT)
		: name(name_),
		  layerID(layerID_) {}
};