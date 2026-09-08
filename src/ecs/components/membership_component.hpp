#pragma once

#include "i_component.hpp"
#include <string>

enum class GroupID {
	DEFAULT,
	SOLDIERS
};

struct MembershipComponent : public IComponent {
	std::string name{};
	GroupID groupID{};

	MembershipComponent(std::string name, GroupID groupID = GroupID::DEFAULT)
		: name(name), groupID(groupID) {}
};