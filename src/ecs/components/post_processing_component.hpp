#pragma once

#include "i_component.hpp"

enum class PostProcessingType {
	INVERSION,
	MORE
};

struct PostProcessingComponent : public IComponent {
	PostProcessingType type{};

	PostProcessingComponent() = default;
	PostProcessingComponent(PostProcessingType type_)
		: type(type_) {}
};