#pragma once

#include "graphics_types.hpp"
#include "mesh.h"
#include <memory>

namespace MeshGenerator {
	Mesh createPlane(float width, float depth, std::shared_ptr<Texture> texture = nullptr, float uvTiling = 1.0f);
	Mesh createCuboid(float width, float depth, float height, std::shared_ptr<Texture> texture = nullptr, float uvTiling = 1.0f);
	Mesh createGrid(float size, float step);
	Mesh createWall(float width, float height, float depth = 0.5f, float uvTiling = 1.0f);
};