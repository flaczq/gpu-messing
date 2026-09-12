#pragma once

#include "../../configs/gl_config.hpp"
#include "i_component.hpp"
#include <memory>

enum class RenderQueueType {
	OPAQUE,
	STENCIL,
	OUTLINE,
	BLENDING,
	TOP_LAYER,
	UI
};

class Model;
class Material;

struct RenderComponent : public IComponent {
	std::shared_ptr<Model> model{};
	std::shared_ptr<Material> material{};
	RenderQueueType queueType{};

	RenderComponent() = default;
	RenderComponent(std::shared_ptr<Model> model_,
					std::shared_ptr<Material> material_,
					RenderQueueType queueType_ = RenderQueueType::OPAQUE)
		: model(model_),
		  material(material_),
		  queueType(queueType_) {}
};