
#include "Shaders.hpp"

using namespace Engine;

CShader::CShader(void) : Type(0), GLShader(0)
{

}

CShader::~CShader(void)
{
    Destroy();
}

bool CShader::Create(std::string Filename)
{
    int Status;

    Filename.insert(0, RootDir + ResourceDir + "shaders\\");
    this->Filename = Filename;

    if (Filename.find(".frag") != std::string::npos) {
        this->Type = GL_FRAGMENT_SHADER;
    } else if (Filename.find(".vert") != std::string::npos) {
        this->Type = GL_VERTEX_SHADER;
    }

    const char* Source = LoadFile(Filename);

    GLShader = glCreateShader(Type);
    glShaderSource(GLShader, 1, &Source, NULL);
    glCompileShader(GLShader);
    glGetShaderiv(GLShader, GL_COMPILE_STATUS, &Status);

    delete [] Source;

    if (Status == GL_TRUE) {
        Logger->Text("Compiled OpenGL Shader - " + Filename);
    } else {
        Logger->Error("Error Compiling OpenGL Shader - " + Filename);
    }

    return glIsShader(GLShader);
}

bool CShader::Destroy(void)
{
    if (glIsShader(GLShader)) {
        glDeleteShader(GLShader);
        Logger->Text("Destroyed OpenGL Shader - " + Filename);
    }

    return !glIsShader(GLShader);
}

const char* CShader::LoadFile(std::string Filename)
{

    std::ifstream FileStream(Filename.c_str(), std::ios::binary | std::ios::in);

    if (FileStream.fail()) {
        Logger->Error("Error Loading Shader File - " + Filename);
    }

    FileStream.seekg(0, std::ios::end);
    int Size = FileStream.tellg();
    FileStream.seekg(0, std::ios::beg);

    char* Buffer = new char[Size + 1];
    FileStream.read(&Buffer[0], Size);
    Buffer[Size] = '\0';

    FileStream.close();

    Logger->Text("Loaded Shader File - " + Filename);

    return Buffer;
}

CProgram::CProgram(void) : GLProgram(0)
{

}

CProgram::~CProgram(void)
{
    Destroy();
}

bool CProgram::Create(const char *Filename, ...)
{
    GLProgram = glCreateProgram();

    va_list Files;
    va_start(Files, Filename);

    while (Filename != NULL) {

        CShader *Shader = new CShader();

        if (Shader->Create(Filename)) {

            glAttachShader(GLProgram, Shader->ShaderID());
            Logger->Text("Attached Shader - " + Shader->GetFilename());

        }

        delete Shader;

        Filename = va_arg(Files, const char*);

    }

    va_end(Files);

    int Linked = 0;

    glLinkProgram(GLProgram);
    glGetProgramiv(GLProgram, GL_LINK_STATUS, &Linked);

    if (Linked == GL_TRUE) {
        Logger->Text("Linked Shader Program");
    } else {
        Logger->Error("Error Linking Shader Program");
    }

    if (glIsProgram(GLProgram)) {
        Logger->Text("Created Shader Program");
    } else {
        Logger->Error("Error Creating Shader Program");
    }

    return glIsProgram(GLProgram);
}

bool CProgram::Destroy(void)
{
    if (glIsProgram(GLProgram)) {
        glDeleteProgram(GLProgram);
        Logger->Text("Destroyed OpenGL Shader Program");
    }

    return !glIsProgram(GLProgram);
}

void CProgram::Bind(void)
{
    glUseProgram(GLProgram);
}

void CProgram::Unbind(void)
{
    glUseProgram(0);
}

int CProgram::SetUniform(std::string Name, float* Values, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform1fv(Location, Count, Values);
    return Location;
}

int CProgram::SetUniform(std::string Name, const float Value)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform1fv(Location, 1, &Value);
    return Location;
}

int CProgram::SetUniform(std::string Name, const float Value1, const float Value2)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform2f(Location, Value1, Value2);
    return Location;
}

int CProgram::SetUniform(std::string Name, glm::vec2* Vecs, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform2fv(Location, Count, (GLfloat*) Vecs);
    return Location;
}

int CProgram::SetUniform(std::string Name, const glm::vec2 Vec)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform2fv(Location, 1, (GLfloat*) &Vec);
    return Location;
}

int CProgram::SetUniform(std::string Name, glm::vec3* Vecs, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform3fv(Location, Count, (GLfloat*) Vecs);
    return Location;
}

int CProgram::SetUniform(std::string Name, const glm::vec3 Vec)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform3fv(Location, 1, (GLfloat*) &Vec);
    return Location;
}

int CProgram::SetUniform(std::string Name, glm::vec4* Vecs, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform4fv(Location, Count, (GLfloat*) Vecs);
    return Location;
}

int CProgram::SetUniform(std::string Name, const glm::vec4 Vec)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform4fv(Location, 1, (GLfloat*) &Vec);
    return Location;
}

int CProgram::SetUniform(std::string Name, glm::mat3* Matrices, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniformMatrix3fv(Location, Count, FALSE, (GLfloat*) Matrices);
    return Location;
}

int CProgram::SetUniform(std::string Name, const glm::mat3 Matrix)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniformMatrix3fv(Location, 1, FALSE, (GLfloat*) &Matrix);
    return Location;
}

int CProgram::SetUniform(std::string Name, glm::mat4* Matrices, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniformMatrix4fv(Location, Count, FALSE, (GLfloat*) Matrices);
    return Location;
}

int CProgram::SetUniform(std::string Name, const glm::mat4 Matrix)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniformMatrix4fv(Location, 1, FALSE, (GLfloat*) &Matrix);
    return Location;
}

int CProgram::SetUniform(std::string Name, int* Values, int Count)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform1iv(Location, Count, Values);
    return Location;
}

int CProgram::SetUniform(std::string Name, const int Value)
{
    int Location = glGetUniformLocation(GLProgram, Name.c_str());
    glUniform1i(Location, Value);
    return Location;
}

void CProgram::SetModelAndNormalMatrix(std::string ModelMaxtrixName, std::string NormalMatrixName, glm::mat4 ModelMatrix)
{
    SetUniform(ModelMaxtrixName, ModelMatrix);
    SetUniform(NormalMatrixName, glm::transpose(glm::inverse(ModelMatrix)));
}

void CProgram::SetModelAndNormalMatrix(std::string ModelMaxtrixName, std::string NormalMatrixName, glm::mat4* ModelMatrix)
{
    SetUniform(ModelMaxtrixName, ModelMatrix);
    SetUniform(NormalMatrixName, glm::transpose(glm::inverse(*ModelMatrix)));
}
