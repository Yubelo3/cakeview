#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <iostream>

class Texture
{
private:
    unsigned int _id;

public:
    Texture(const char *filepath, int format=GL_RGB)
    {
        // 加载纹理数据
        stbi_set_flip_vertically_on_load(true);
        int width, height, channel;
        unsigned char *data = stbi_load(filepath, &width, &height, &channel, 0);
        if (!data)
        {
            std::cerr << "Failed to load texture image" << std::endl;
            exit(-1);
        }

        // 绑定纹理对象，加载
        glGenTextures(1, &_id);
        glBindTexture(GL_TEXTURE_2D, _id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // 对象，Mipmap层数，存储格式，宽，高，无意义参数，源图存储格式，源图数据类型，数据
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        // 设置
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

public:
    inline void bind(unsigned int slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, _id);
    }
    inline void unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};