#include "../../libs/stb_image.h"
#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "texture_primitive.h"
#include <iostream>

namespace TexturePrimitive {
    unsigned int load(const char* path) {
        //    ┏┓┏┓┳┓┏┓┳┓┏┓┏┳┓•┳┓┏┓
        //    ┃┓┣ ┃┃┣ ┣┫┣┫ ┃ ┓┃┃┃┓
        //    ┗┛┗┛┛┗┗┛┛┗┛┗ ┻ ┗┛┗┗┛
        //                        
        unsigned int textureID;
        glGenTextures(1, &textureID);
        //stbi_set_flip_vertically_on_load(true);

        int data_w, data_h, data_ch;
        unsigned char* data = stbi_load(path, &data_w, &data_h, &data_ch, 0);
        if (data) {
            // data_ch == 1
            GLenum format = GL_RED;
            if (data_ch == 3) {
                format = GL_RGB;
            }
            else if (data_ch == 4) {
                format = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, data_w, data_h, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            //    ┏┓•┓ ┏┳┓┏┓┳┓  ┏┓┏┓┳┓┏┓┳┳┓┏┓
            //    ┣ ┓┃  ┃ ┣ ┣┫  ┃┃┣┫┣┫┣┫┃┃┃┗┓
            //    ┻ ┗┗┛ ┻ ┗┛┛┗  ┣┛┛┗┛┗┛┗┛ ┗┗┛
            //                               
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else {
            LOG_E("TEXTURE_PRIMITIVE::LOADED_FAILED: " << path);
        }

        stbi_image_free(data);

        return textureID;
    }

    void bind(unsigned int textureID, unsigned int i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    void clean(unsigned int& textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}