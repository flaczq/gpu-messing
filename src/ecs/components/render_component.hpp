#pragma once

#include "i_component.hpp"
#include <memory>

class Model;
class Material;

struct RenderComponent : public IComponent {
	std::shared_ptr<Model> model;
	std::shared_ptr<Material> material;
};