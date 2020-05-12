
#ifndef CLOGGER_HPP
#define CLOGGER_HPP

#include <windows.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>

extern std::string ResourceDir;
extern std::string RootDir;

extern bool Loop;
extern bool Destroy(void);

class CLogger
{

private:

    std::fstream LogFile;

public:

    CLogger(void);
    CLogger(std::string Filename);
    ~CLogger(void);

    bool New(std::string Filename);
    bool Open(std::string Filename);
    void Close(void);

    void Text(std::string Text, bool Console = true, bool Msgbox = false);
    void Error(std::string Error, bool Console = true, bool Msgbox = true, bool Exit = true);

};

#endif
