#include "texture.h"

Texture::Texture(const GLchar* path) {
    //    ┏┓┏┓┳┓┏┓┳┓┏┓┏┳┓•┳┓┏┓
    //    ┃┓┣ ┃┃┣ ┣┫┣┫ ┃ ┓┃┃┃┓
    //    ┗┛┗┛┛┗┗┛┛┗┛┗ ┻ ┗┛┗┗┛
    //                        
    glGenTextures(1, &ID);

    int data_w, data_h, data_ch;
    data = stbi_load(path, &data_w, &data_h, &data_ch, 0);
    if (data) {
        // data_ch == 1
        GLenum format = GL_RED;
        if (data_ch == 3) {
            format = GL_RGB;
        } else if (data_ch == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, ID);
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
    } else {
        std::cout << "ERROR::TEXTURE::NOT_LOADED_SUCCESFULLY: " << path << std::endl;
    }
}

Texture::~Texture() {
    if (data) {
        stbi_image_free(data);
    }
}

unsigned int Texture::getTexture() {
    return ID;
}