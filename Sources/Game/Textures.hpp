


#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#define GLEW_STATIC

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>
#include <Soil2/Soil2.h>

#include "Pods.hpp"

namespace Game
{

class CTexture
{

private:

    bool ActPalette;
    GLuint GLTexture;
    std::string Filename;

public:

    CTexture(void);
    ~CTexture(void);

    bool LoadRaw(CPodPool &PodPool, std::string Filename, unsigned int *DefaultPalette = NULL);
    bool LoadBlank(GLuint Dimensions, GLuint Color, GLenum Format);
    bool LoadImage(std::string Filename, std::string AlternateDir = "");
    bool LoadMemory(GLubyte *Data, GLuint Dimensions, GLuint Bpp, GLenum Format);

    void Destroy(void);

    void SetParameters(GLint MinFilter, GLint MagFilter, GLint WrapS, GLint WrapT);

    void Bind(GLuint TextureUnit);
    void Unbind(void)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint &GetTexture(void)
    {
        return GLTexture;
    }
    std::string &GetFilename(void)
    {
        return Filename;
    }
    bool &GetPalette(void)
    {
        return ActPalette;
    }

};

class CTexturePool
{

private:

    unsigned int DefaultPalette[256];
    glm::vec2 TextureCoords[2][2][4][4];

    std::vector<CTexture> Textures;

public:

    CTexturePool(void);
    ~CTexturePool(void);

    bool Create(CPodPool &PodPool, std::vector<std::string> &Filenames);

    void Bind(int Index)
    {
        Textures[Index].Bind(0);
    }

    const GLuint GetTexture(int Index)
    {
        return Textures[Index].GetTexture();
    }

    int NumTextures(void) {
        return Textures.size();
    }
    const std::string &GetFilename(int Index)
    {
        return Textures[Index].GetFilename();
    }
    const bool &GetPalette(int Index)
    {
        return Textures[Index].GetPalette();
    }
    glm::vec2 &GetTextureCoords(int MirrorX, int MirrorY, int Rotation, int Corner);

};

}

#endif

