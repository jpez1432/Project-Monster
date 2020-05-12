

#include "Textures.hpp"

using namespace Game;

CTexture::CTexture(void) : GLTexture(0), Filename("Filename...")
{

}

CTexture::~CTexture(void)
{
    Destroy();
}

bool CTexture::LoadBlank(GLuint Dimensions, GLuint Color, GLenum Format)
{
    std::vector<GLubyte>Data(Dimensions * Dimensions * 4, Color);

    glGenTextures(1, &GLTexture);
    glBindTexture(GL_TEXTURE_2D, GLTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, Format, Dimensions, Dimensions, 0, Format, GL_UNSIGNED_BYTE, &Data[0]);

    Data.clear();

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Dimensions, Dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, &Data[0]);

    if(glIsTexture(GLTexture)) {
        Logger->Text("Created New Blank OpenGL Texture");
    } else {
        Logger->Error("Error Creating New Blank OpenGL Texture");
    }

    return glIsTexture(GLTexture);

}

bool CTexture::LoadRaw(CPodPool &PodPool, std::string Filename, unsigned int *DefaultPalette)
{

    int Size, Offset;
    this->Filename = Filename;

    unsigned char ColorTable[768];
    unsigned int ExpandedTable[256];

    std::string Palette = "Art\\" + Filename.substr(0, Filename.length() - 3) + "Act";
    PodPool.FindFile(Palette, Size, Offset);

    Palette.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Palette.c_str(), std::ios::binary | std::ios::in);

    if(!StreamIn.fail()) {

        ActPalette = true;

        StreamIn.seekg(Offset, std::ios::beg);
        StreamIn.read((char*)ColorTable, sizeof(ColorTable));
        StreamIn.close();

        for(int i = 0; i < 256; i++) {

            ExpandedTable[i] = ColorTable[i * 3 + 2] << 16 | ColorTable[i * 3 + 1] << 8 | ColorTable[i * 3 + 0];

            if(ColorTable[i * 3 + 0] >= 8 || ColorTable[i * 3 + 1] >= 8 || ColorTable[i * 3 + 2] >= 8) {
                ExpandedTable[i] |= 0xFF000000;
            }

        }

    } else {

        ActPalette = false;

        if (DefaultPalette == NULL) {

            DefaultPalette = new unsigned int[256];

            Palette = RootDir + ResourceDir + "\\MetalCr2.act";
            std::ifstream StreamIn(Palette.c_str(), std::ios::binary | std::ios::in);

            if(StreamIn.fail()) {
                return false;
            }

            StreamIn.read((char*)ColorTable, 768);
            StreamIn.close();

            for(int i = 0; i < 256; i++) {

                DefaultPalette[i] = ColorTable[i * 3 + 2] << 16 | ColorTable[i * 3 + 1] << 8 | ColorTable[i * 3 + 0];

                if(ColorTable[i * 3 + 0] >= 8 || ColorTable[i * 3 + 1] >= 8 || ColorTable[i * 3 + 2] >= 8) {
                    DefaultPalette[i] |= 0xFF000000;
                }

            }
        }
    }

    Filename.insert(0, "Art\\");
    PodPool.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Size = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    unsigned char *Buffer = new unsigned char[Size];
    StreamIn.read((char*)Buffer, Size);

    StreamIn.close();

    Size = sqrt(Size);
    unsigned int *Data = new unsigned int[Size * Size * 4];

    if(!ActPalette) {

        for(int z = 0; z < Size; z++) {
            for(int x = 0; x < Size; x++) {
                Data[(z + Size * x)] = DefaultPalette[Buffer[(z + Size * x)]];
            }
        }

    } else {

        for(int z = 0; z < Size; z++) {
            for(int x = 0; x < Size; x++) {
                Data[(z + Size * x)] = ExpandedTable[Buffer[(z + Size * x)]];
            }
        }

    }

    delete [] Buffer;

    glGenTextures(1, &GLTexture);
    glBindTexture(GL_TEXTURE_2D, GLTexture);

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Size, Size, 0, GL_RGBA, GL_UNSIGNED_BYTE, &Data[0]);

    delete [] Data;

    return glIsTexture(GLTexture);

}

bool CTexture::LoadImage(std::string Filename, std::string AlternateDir)
{

    if(AlternateDir != "") {
        Filename.insert(0, RootDir + ResourceDir + AlternateDir);
    } else {
        Filename.insert(0, RootDir + ResourceDir + "Textures\\");
    }

    if(Config->Graphics.TextureQuality == 2) {
        GLTexture = SOIL_load_OGL_texture(Filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
    } else {
        GLTexture = SOIL_load_OGL_texture(Filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    }

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    if(glIsTexture(GLTexture)) {
        Logger->Text("Loaded OpenGL Texture - " + Filename);
    } else {
        Logger->Error("Error Loading OpenGL Texture - " + Filename);
    }

    return glIsTexture(GLTexture);

}

bool CTexture::LoadMemory(GLubyte *Data, GLuint Dimensions, GLuint Bpp, GLenum Format)
{

    glGenTextures(1, &GLTexture);
    glBindTexture(GL_TEXTURE_2D, GLTexture);

    if(Config->Graphics.TextureQuality == 0) {
        SetParameters(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 1) {
        SetParameters(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
    } else if(Config->Graphics.TextureQuality == 2) {
        SetParameters(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Dimensions, Dimensions, 0, GL_RGBA, GL_UNSIGNED_BYTE, &Data[0]);

    if(glIsTexture(GLTexture)) {
        Logger->Text("Loaded OpenGL Texture From Memory");
    } else {
        Logger->Error("Error Loading OpenGL Texture From Memory");
    }

    return glIsTexture(GLTexture);

}

void CTexture::Destroy(void)
{
    if(glIsTexture(GLTexture)) {
        glDeleteTextures(1, &GLTexture);
        Logger->Text("Released OpenGL Texture Memory - " + Filename);
    }
}

void CTexture::Bind(GLuint TextureUnit)
{
    glActiveTexture(GL_TEXTURE0 + TextureUnit);
    glBindTexture(GL_TEXTURE_2D, GLTexture);
}

void CTexture::SetParameters(GLint MinFilter, GLint MagFilter, GLint WrapS, GLint WrapT)
{

    if(glIsTexture(GLTexture)) {

        glBindTexture(GL_TEXTURE_2D, GLTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);

    }
}

CTexturePool::CTexturePool(void)
{

    const double TEX_0 = 0.03123f;
    const double TEX_1 = 1.0f - 0.03123f;

    glm::vec2 MirrorRotationCoords[2][2][4][4] = {
        {
            {
                {glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_1, TEX_1)},
                {glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_1, TEX_0)},
                {glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_0, TEX_0)},
                {glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_0, TEX_1)}
            },
            {
                {glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_1, TEX_0)},
                {glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_0, TEX_0)},
                {glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_0, TEX_1)},
                {glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_1, TEX_1)}
            }
        },
        {
            {
                {glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_0, TEX_1)},
                {glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_1, TEX_1)},
                {glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_1, TEX_0)},
                {glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_0, TEX_0)}
            },
            {
                {glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_0, TEX_0)},
                {glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_0, TEX_1)},
                {glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_1, TEX_0), glm::vec2(TEX_1, TEX_1)},
                {glm::vec2(TEX_1, TEX_1), glm::vec2(TEX_0, TEX_1), glm::vec2(TEX_0, TEX_0), glm::vec2(TEX_1, TEX_0)}
            }
        }
    };

    memcpy(TextureCoords, MirrorRotationCoords, sizeof(TextureCoords));

}

CTexturePool::~CTexturePool(void)
{
    if(!Textures.empty()) {
        Textures.clear();
    }
}

bool CTexturePool::Create(CPodPool &PodPool, std::vector<std::string> &Filenames)
{
    Textures.reserve(512);

    int Size, Offset;
    unsigned char ColorTable[768];

    std::string Palette("Art\\MetalCr2.act");
    PodPool.FindFile(Palette, Size, Offset);

    Palette.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Palette.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, std::ios::beg);
    StreamIn.read((char*)ColorTable, 768);
    StreamIn.close();

    for(unsigned int i = 0; i < 256; i++) {

        DefaultPalette[i] = ColorTable[i * 3 + 2] << 16 | ColorTable[i * 3 + 1] << 8 | ColorTable[i * 3 + 0];

        if(ColorTable[i * 3 + 0] > 8 || ColorTable[i * 3 + 1] > 8 || ColorTable[i * 3 + 2] > 8) {
            DefaultPalette[i] |= 0xFF000000;
        }

    }

    Textures.resize(Filenames.size());

    for(unsigned int i = 0; i < Textures.size(); i++) {

        if(Textures[i].LoadRaw(PodPool, Filenames[i], DefaultPalette)) {
            Logger->Text("Loaded Raw Texture - " + Filenames[i]);
        } else {
            Logger->Error("Error Loading Raw Texture - " + Filenames[i]);
        }

    }

    return true;

}

glm::vec2 &CTexturePool::GetTextureCoords(int MirrorX, int MirrorY, int Rotation, int Corner)
{

    return TextureCoords[MirrorX][MirrorY][Rotation][Corner];
}

