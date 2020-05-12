
#include "Logger.hpp"

CLogger::CLogger(void)
{

}

CLogger::CLogger(std::string Filename)
{

    New(Filename);

}

CLogger::~CLogger(void)
{

    Close();

}

bool CLogger::New(std::string Filename)
{

    if(LogFile.is_open()) {
        LogFile.close();
    }

    LogFile.open(Filename.c_str(), std::ios::out | std::ios::trunc);

    return LogFile.is_open();

}

bool CLogger::Open(std::string Filename)
{

    LogFile.open(Filename.c_str(), std::ios::out | std::ios::trunc);

    return LogFile.is_open();

}

void CLogger::Close(void)
{

    LogFile.close();

}

void CLogger::Text(std::string Text, bool Console, bool Msgbox)
{

    if(LogFile.is_open()) {

        std::cout << Text << std::endl;

        if(Console) {
            LogFile << Text << std::endl;
        }

        if(Msgbox) {
            MessageBox(NULL, Text.c_str(), "Log:", MB_ICONINFORMATION);
        }

    }

}

void CLogger::Error(std::string Error, bool Console, bool Msgbox, bool Exit)
{

    if(LogFile.is_open()) {

        if(Console) {
            LogFile << Error << std::endl;
        }

        if(Msgbox) {
            MessageBox(NULL, Error.c_str(), "Error:", MB_ICONINFORMATION);
        }

        if(Exit) {
//            Loop = false;
            Destroy();
            exit(0);
        }

    }

}


