
#ifndef PODS_HPP
#define PODS_HPP

#include "../Common.hpp"
#include <string>
#include <vector>

#include "../Logger.hpp"
#include "../Config.hpp"

extern CLogger *Logger;
extern CConfig *Config;

extern std::string ResourceDir;
extern std::string RootDir;

namespace Game
{

class CPod
{

private:

    int NumFiles;
    char Comment[80];
    std::string PodFile;

    struct Directory {
        char Filename[32];
        int Size;
        int Offset;
    };

    std::vector<Directory> Directories;

public:

    CPod(void);
    ~CPod(void);

    bool Load(std::string Filename);

    std::string &Filename(void)
    {
        return PodFile;
    }
    const int &NumberOfFiles(void)
    {
        return NumFiles;
    }
    const char *PodComment(void)
    {
        return Comment;
    }

    const char *Filename(int Index)
    {
        return Directories[Index].Filename;
    }
    const int &Filesize(int Index)
    {
        return Directories[Index].Size;
    }
    const int &FileOffset(int Index)
    {
        return Directories[Index].Offset;
    }

};

class CPodPool
{

private:

    struct Info {
        char Filename[32];
        char Name[32];
    };

    bool Truck;
    bool Track;

    std::vector<CPod> Pods;
    std::vector<Info> Trucks;
    std::vector<Info> Tracks;

public:

    CPodPool(void);
    ~CPodPool(void);

    bool Create(std::string Filename);

    bool FindFile(std::string &Filename, int &Size, int &Offset);
    bool Prioritize(std::string Filename, int Element);

    bool EnumerateFiles(void);

    const unsigned int NumberOfFiles(void)
    {
        return Pods.size();
    }
    const std::string Filename(int Index)
    {
        return Pods[Index].Filename();
    }

    const unsigned int NumTrucks(void)
    {
        return Trucks.size();
    }
    char &TruckFilename(int Index)
    {
        return Trucks[Index].Filename[0];
    }
    char &TruckName(int Index)
    {
        return Trucks[Index].Name[0];
    }

    const unsigned int NumTracks(void)
    {
        return Tracks.size();
    }
    char &TrackFilename(int Index)
    {
        return Tracks[Index].Filename[0];
    }
    char &TrackName(int Index)
    {
        return Tracks[Index].Name[0];
    }

};

}

#endif
