
#include "Font.hpp"

using namespace Engine;

CFont::CFont(void) : GLFont(0)
{
}

CFont::~CFont(void)
{
    Destroy();
}

bool CFont::Create(std::string Filename, int Size, bool Bold, bool Italic, bool Underline, bool Strike)
{
    HFONT Font;
    GLFont = glGenLists(96);

    Font = CreateFont(Size, 0, 0, 0, Bold ? FW_BOLD : 0, Italic, Underline, Strike, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, Filename.c_str());

    if(!Font) {
        return false;
    }

    SelectObject(wglGetCurrentDC(), Font);
    wglUseFontBitmaps(wglGetCurrentDC(), 32, 96, GLFont);

    return true;
}

void CFont::Destroy(void)
{
    if(glIsList(GLFont)) {
        glDeleteLists(GLFont, 1);
    }
}

void CFont::Render(std::string Text, glm::vec2 Position, glm::vec4 Color)
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    glColor4f(Color.r, Color.g, Color.b, Color.a);
    glRasterPos2f(Position.x, Position.y);

    glPushAttrib(GL_LIST_BIT);

    glListBase(GLFont - 32);
    glCallLists(GLsizei(Text.length()), GL_UNSIGNED_BYTE, Text.c_str());

    glPopAttrib();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
}
