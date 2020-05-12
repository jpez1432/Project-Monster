
#ifndef COMMON_HPP
#define COMMON_HPP

#include <windows.h>
#include <string>
#include <vector>
#include <glm.hpp>

std::string GetInfo();
std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::vector<std::string> ListFiles(std::string Folder, std::string Extensions);
bool StrCompare(const std::string& Str1, const std::string& Str2);
void ParseVector(std::string Line, glm::vec3 &Vector);


#endif

