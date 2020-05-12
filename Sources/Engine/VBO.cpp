
#include "VBO.hpp"

using namespace Engine;

CVBO::CVBO(void) : Buffer(0)
{
}

CVBO::~CVBO(void)
{
    Destroy();
}

void CVBO::Create(int DefaultSize)
{
    glGenBuffers(1, &Buffer);
    Data.reserve(DefaultSize);
    Size = DefaultSize;
    CurrentSize = 0;
}

bool CVBO::Destroy(void)
{
    if (glIsBuffer(Buffer)) {
        glDeleteBuffers(1, &Buffer);
        Data.clear();
        return true;
    }

    return false;
}

void* CVBO::MapBufferToMemory(GLint UsageHint)
{
    void* PtrRes = glMapBuffer(Type, UsageHint);
    return PtrRes;
}

void* CVBO::MapSubBufferToMemory(GLint UsageHint, GLuint Offset, GLuint Length)
{
    void* ptrRes = glMapBufferRange(Type, Offset, Length, UsageHint);
    return ptrRes;
}

void CVBO::UnmapBuffer(void)
{
    glUnmapBuffer(Type);
}

void CVBO::Bind(GLint BufferType)
{
    Type = BufferType;
    glBindBuffer(Type, Buffer);
}

void CVBO::UploadDataToGPU(GLint DrawingHint)
{
    glBufferData(Type, Data.size(), &Data[0], DrawingHint);
    Data.clear();
}

void CVBO::AddData(void* PtrData, GLuint DataSize)
{
    Data.insert(Data.end(), (char*) PtrData, (char*) PtrData + DataSize);
    CurrentSize += DataSize;
}

void* CVBO::GetDataPointer(void)
{
    return (void*) &Data[0];
}
