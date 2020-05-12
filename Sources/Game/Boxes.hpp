
#ifndef BOXES_HPP
#define BOXES_HPP

#include "Pods.hpp"

extern std::string ResourceDir;
extern std::string RootDir;

namespace Game
{

class CBoxes
{

private:

    int Dimensions;

    typedef struct {
        unsigned char Indices[6];
        unsigned char Rotations[6];
    } Information;

    std::vector<Information> TextureInfo;
    std::vector<unsigned char> LowerHeights;
    std::vector<unsigned char> UpperHeights;

    const int IndexAt(int X, int Z);

public:

    CBoxes(void);
    ~CBoxes(void);

    bool Load(Game::CPodPool &PodPool, std::string Filename, std::string Info);

    const int GetTextureIndex(int X, int Z, int Side);
    void GetTextureRotation(int X, int Z, int Side, int &MirrorX, int &MirrorY, int &Rotation);
    const float GetLowerHeight(int X, int Z);
    const float GetUpperHeight(int X, int Z);

};

}

#endif
