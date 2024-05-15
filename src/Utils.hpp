#pragma once
#include <vector>
#include <iostream>

template <typename T>
std::vector<T> loadArray(const char *filepath)
{
    std::ifstream ifs(filepath);
    if (!ifs.is_open())
    {
        std::cerr << "Failed to open " << filepath << std::endl;
        exit(-1);
    }
    std::vector<T> data;

    T tmp;
    while (ifs >> tmp)
        data.push_back(tmp);
    return data;
}

void errorCheck()
{
    while (GLenum error = glGetError())
        std::cerr << "[GL ERROR]: 0x" << std::hex << error << std::endl;
}
