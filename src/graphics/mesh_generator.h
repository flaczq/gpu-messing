#pragma once

#include "mesh.h"
#include <vector>

namespace MeshGenerator {
	Mesh createPlane(float width, float depth, float uvTiling = 1.0f);
	Mesh createCuboid(float width, float depth, float height, float uvTiling = 1.0f);
	std::vector<Mesh> createRoom(float width, float height, float depth, float uvTiling = 1.0f);
};