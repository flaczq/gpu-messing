#pragma once

#include "../utils/commongl.h"
#include "../../libs/stb_image.h"

class Texture {
public:
	Texture(const GLchar* path);
	~Texture();

	unsigned int getTexture();

private:
	unsigned int ID;
	unsigned char* data;
};