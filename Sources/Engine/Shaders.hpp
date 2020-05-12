
#ifndef SHADERS_HPP
#define SHADERS_HPP

#include "../Game/Pods.hpp"

namespace Engine
{

class CShader
{

private:

    int Type;
    GLuint GLShader;
    std::string Filename;

    const char* LoadFile(std::string Filename);

public:

    CShader(void);
    ~CShader(void);

    bool Create(std::string Filename);
    bool Destroy(void);

    const GLuint ShaderID(void)
    {
        return GLShader;
    }
    std::string GetFilename(void)
    {
        return Filename;
    }

};

class CProgram
{

private:

    GLuint GLProgram;
    std::vector<Engine::CShader> Shaders;

public:

    CProgram(void);
    ~CProgram(void);

    bool Create(const char *Filename, ...);
    bool Destroy(void);

    void Bind(void);
    void Unbind(void);

    const GLuint ProgramID(void)
    {
        return GLProgram;
    }

    int SetUniform(std::string Name, glm::vec2* Vecs, int Count = 1);
    int SetUniform(std::string Name, const glm::vec2 Vec);
    int SetUniform(std::string Name, glm::vec3* Vec, int Count = 1);
    int SetUniform(std::string Name, const glm::vec3 Vec);
    int SetUniform(std::string Name, glm::vec4* Vecs, int Count = 1);
    int SetUniform(std::string Name, const glm::vec4 Vec);
    int SetUniform(std::string Name, float* Values, int Count = 1);
    int SetUniform(std::string Name, const float Value);
    int SetUniform(std::string Name, const float Value1, const float Value2);
    int SetUniform(std::string Name, glm::mat3* Matrices, int Count = 1);
    int SetUniform(std::string Name, const glm::mat3 Matrix);
    int SetUniform(std::string Name, glm::mat4* Matrices, int Count = 1);
    int SetUniform(std::string Name, const glm::mat4 Matrix);
    int SetUniform(std::string Name, int* Values, int Count = 1);
    int SetUniform(std::string Name, const int Value);
    void SetModelAndNormalMatrix(std::string ModelMaxtrixName, std::string NormalMatrixName, glm::mat4 ModelMatrix);
    void SetModelAndNormalMatrix(std::string ModelMaxtrixName, std::string NormalMatrixName, glm::mat4* ModelMatrix);

};
}

#endif
