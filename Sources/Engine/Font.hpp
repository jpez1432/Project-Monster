

#ifndef FONT_HPP
#define FONT_HPP

#include "../common.hpp"

namespace Engine
{

class CFont
{

private:

    GLuint GLFont;

public:

    CFont(void);
    ~CFont(void);

    bool Create(std::string Filename, int Size,  bool Bold, bool Italic, bool Underline, bool Strike);
    void Destroy(void);

    void Render(std::string Text, glm::vec2 Position, glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

};
}

#endif

