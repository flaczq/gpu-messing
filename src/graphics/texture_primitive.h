#pragma once

#include "../utils/config.h"

// do not store data, just load/bind/clean
namespace TexturePrimitive {
	unsigned int load(const char* path);
	void bind(unsigned int textureID, unsigned int slot);
	void clean(unsigned int& textureID);
};