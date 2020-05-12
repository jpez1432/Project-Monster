
#ifndef VBO_HPP
#define VBO_HPP

#define GLEW_STATIC

#include <vector>
#include <GL/Glew.h>

#include "../Common.hpp"

namespace Engine
{

class CVBO
{

private:

    GLint Size;
    GLint CurrentSize;
    GLint Type;
    GLuint Buffer;
    bool Uploaded;

    std::vector<GLubyte>Data;

public:

    void* MapBufferToMemory(GLint UsageHint);
    void* MapSubBufferToMemory(GLint UsageHint, GLuint Offset, GLuint Length);
    void UnmapBuffer(void);
    void AddData(void* PtrData, GLuint DataSize);
    void UploadDataToGPU(int DrawingHint);

    void Bind(GLint BufferType = GL_ARRAY_BUFFER);

    void* GetDataPointer(void);
    GLuint GetBufferID(void)
    {
        return Buffer;
    }
    int GetCurrentSize(void)
    {
        return CurrentSize;
    }

public:

    CVBO(void);
    ~CVBO(void);

    void Create(int DefaultSize = 0);
    bool Destroy(void);

};
}

#endif
