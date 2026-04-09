#pragma once

#include "mesh.h"

namespace MeshGenerator {
	Mesh createPlane(float width, float depth, float uvTiling = 1.0f);
	Mesh createCube(float width, float depth, float height);
};