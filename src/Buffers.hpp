#pragma once
#include <GL/glew.h>
#include <vector>

struct VertexBufferLayoutElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalize;
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferLayoutElement> _elem;
    unsigned int _stride=0;

private:
    static unsigned int sizeofType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;        
        default:
            return -1;
        }
    }

public:
    VertexBufferLayout()=default;

public:
    void addElem(unsigned int type,unsigned int count,unsigned char normalize)
    {
        _elem.push_back({type,count,normalize});
        _stride+=sizeofType(type)*count;
    }
    void bind() const
    {
        unsigned int offset=0;
        for(unsigned int i=0;i<_elem.size();i++)
        {
            const auto& l=_elem[i];
            glVertexAttribPointer(i,l.count,l.type,l.normalize,_stride,(const void*)offset);
            glEnableVertexAttribArray(i);
            offset+=l.count*sizeofType(l.type);
        }
    }
};

class VertexBuffer
{
private:
    unsigned int _id;

public:
    VertexBuffer(){};
    VertexBuffer(const void* data, unsigned int size)
    {
        glGenBuffers(1,&_id);
        this->bind();
        glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
        //GL_STATIC_DRAW/GL_DYNAMIC_DRAW/GL_STREAM_DRAW指出了数据变化的频率
    }
    inline void bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER,_id);
    }
    inline void unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
};

class VertexArray
{
private:
    unsigned int _id;

public:
    VertexArray()
    {
        glGenVertexArrays(1,&_id);
    }

public:
    inline void bind() const
    {
        glBindVertexArray(_id);
    }
    inline void unbind() const
    {
        glBindVertexArray(0);
    }
};

class IndexBuffer
{
private:
    unsigned int _id;

public:
    IndexBuffer(unsigned int* data,unsigned int size)
    {
        glGenBuffers(1,&_id);
        this->bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
    }

public:
    inline void bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_id);
    }
    inline void unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
};