#pragma once

#include "graphics_types.hpp"
#include "mesh.h"
#include <memory>
#include <vector>

namespace MeshGenerator {
	Mesh createPlane(float width, float depth, std::shared_ptr<Texture> texture = nullptr, float uvTiling = 1.0f);
	Mesh createCuboid(float width, float depth, float height, std::shared_ptr<Texture> texture = nullptr, float uvTiling = 1.0f);
	std::vector<Mesh> createRoom(float width, float height, float depth, float uvTiling = 1.0f);
};