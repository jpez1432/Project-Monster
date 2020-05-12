
#include "Boxes.hpp"

using namespace Game;

CBoxes::CBoxes(void) : Dimensions(0)
{


}

CBoxes::~CBoxes(void)
{

    if(TextureInfo.empty() == false) {
        TextureInfo.clear();
        Logger->Text("Released Box Texture Information Memory");
    }

    if(LowerHeights.empty() == false) {
        LowerHeights.clear();
        Logger->Text("Released Box Lower Height Memory");
    }

    if(UpperHeights.empty() == false) {
        UpperHeights.clear();
        Logger->Text("Released Box Upper Height Memory");
    }

}

bool CBoxes::Load(CPodPool &PodPool, std::string Filename, std::string Info)
{

    int Offset;

    std::string LowerFilename = "Data\\" + Filename.substr(0, Filename.length() - 3) + "Ra0";
    PodPool.FindFile(LowerFilename, Dimensions, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    LowerFilename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(LowerFilename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Dimensions = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Dimensions);
    LowerHeights.resize(Dimensions * Dimensions);

    StreamIn.read((char*)&LowerHeights[0], LowerHeights.size());
    StreamIn.close();

    std::string UpperFilename = "Data\\" + Filename.substr(0, Filename.length() - 3) + "Ra1";
    PodPool.FindFile(UpperFilename, Dimensions, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    UpperFilename.insert(0, RootDir + ResourceDir);
    StreamIn.open(UpperFilename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Dimensions = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Dimensions);
    UpperHeights.resize(Dimensions * Dimensions);

    StreamIn.read((char*)&UpperHeights[0], UpperHeights.size());
    StreamIn.close();

    std::string InfoFilename = "Data\\" + Filename.substr(0, Filename.length() - 3) + "Cl0";
    PodPool.FindFile(InfoFilename, Dimensions, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    InfoFilename.insert(0, RootDir + ResourceDir);
    StreamIn.open(InfoFilename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Dimensions = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Dimensions / 12);
    TextureInfo.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            for(int i = 0; i < 6; i++) {

                TextureInfo[IndexAt(x, z)].Indices[i] = StreamIn.get();
                TextureInfo[IndexAt(x, z)].Rotations[i] = StreamIn.get();

            }
        }
    }

    StreamIn.close();

    return true;

}

const int CBoxes::IndexAt(int X, int Z)
{
    return int (((Z + Dimensions) % Dimensions) + ((X + Dimensions) % Dimensions) * Dimensions);
}

const float CBoxes::GetLowerHeight(int X, int Z)
{
    return float(LowerHeights[IndexAt(X, Z)] / 16.0f);
}

const float CBoxes::GetUpperHeight(int X, int Z)
{
    return float(UpperHeights[IndexAt(X, Z)] / 16.0f);
}

const int CBoxes::GetTextureIndex(int X, int Z, int Side)
{
    return int(TextureInfo[IndexAt(X, Z)].Indices[Side] + ((TextureInfo[IndexAt(X, Z)].Rotations[Side] & 1) << 8));
}

void CBoxes::GetTextureRotation(int X, int Z, int Side, int &MirrorX, int &MirrorY, int &Rotation)
{
    MirrorX = (TextureInfo[IndexAt(X, Z)].Rotations[Side] & 32) >> 5;
    MirrorY = (TextureInfo[IndexAt(X, Z)].Rotations[Side] & 16) >> 4;
    Rotation = (TextureInfo[IndexAt(X, Z)].Rotations[Side] & 192) >> 6;
}




