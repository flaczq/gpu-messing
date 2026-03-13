#pragma once

#include "../utils/commongl.h"

namespace Texture {
	unsigned int load(const char* path);
	void bind(unsigned int textureID, unsigned int slot);
	void clean(unsigned int& textureID);
};