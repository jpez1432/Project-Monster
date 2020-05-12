
#include "Common.hpp"

#ifdef _DEBUG32
const char *Build = "[x86 Debug]";
#elif _RELEASE32
const char *Build = "[x86 Release]";
#elif _DEBUG64
const char *Build = "[x64 Debug]";
#elif _RELEASE64
const char *Build = "[x64 Release]";
#endif

static inline bool CaseInsCharCompareN(char A, char B) { return(std::toupper(A) == std::toupper(B)); }

std::string GetInfo()
{
    TCHAR VersionFile[MAX_PATH];
    GetModuleFileName(NULL, VersionFile, MAX_PATH);

    char Info[64];
    DWORD Handle = 0;
    UINT Size = 0;
    LPBYTE Buffer = NULL;
    DWORD VersionSize = GetFileVersionInfoSize(VersionFile, &Handle);

    if(VersionSize != 0) {
        LPSTR VersionData = new char[VersionSize];
        if(GetFileVersionInfo(VersionFile, Handle, VersionSize, VersionData)) {
            if(VerQueryValue(VersionData, "\\", (VOID FAR * FAR*)&Buffer, &Size)) {
                if(Size) {
                    VS_FIXEDFILEINFO *VersionInfo = (VS_FIXEDFILEINFO *)Buffer;
                    if(VersionInfo->dwSignature == 0xfeef04bd) {
                        sprintf(Info, "Project Monster - v%d.%d.%d %s",
                                int(VersionInfo->dwFileVersionMS >> 16) & 0xffff,
                                int(VersionInfo->dwFileVersionMS >>  0) & 0xffff,
                                int(VersionInfo->dwFileVersionLS >> 16) & 0xffff,
                                Build
                               );
                    }
                }
            }
        }
        delete[] VersionData;
    }

    return std::string(Info);
}

std::vector<std::string> ListFiles(std::string Folder, std::string Extensions)
{
    std::vector<std::string> Filenames;
    std::string Path = (Folder + "/" + Extensions);

    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile(Path.c_str(), &fd);

    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                Filenames.push_back(fd.cFileName);
            }
        } while(::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }

    return Filenames;
}

std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt)
{

    OPENFILENAME ofn;
    char szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrDefExt = DefaultExt;
    ofn.lpstrInitialDir = InitPath;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetOpenFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    } else {
        return "";
    }
}

std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt)
{
    OPENFILENAME ofn;
    char szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrDefExt = DefaultExt;
    ofn.lpstrInitialDir = InitPath;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetSaveFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    } else {
        return "";
    }
}

bool StrCompare(const std::string& Str1, const std::string& Str2) {
    return ((Str1.size() == Str2.size()) && equal(Str1.begin(), Str1.end(), Str2.begin(), CaseInsCharCompareN));
}

void ParseVector(std::string Line, glm::vec3 &Vector)
{
    Vector.x = atof(Line.substr(Line.find_last_of(",") + 1, Line.length()).c_str());
    Vector.y = atof(Line.substr(Line.find_first_of(",") + 1, Line.find_last_of(",") - 1 - Line.find_first_of(",") + 1).c_str());
    Vector.z = atof(Line.substr(0, Line.find_first_of(",")).c_str());
}



