
#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>
#include <string>

#include "Pods.hpp"

namespace Game
{

class CTerrain
{

private:

    int Dimensions;
    float MinHeight;
    float MaxHeight;

    typedef struct {
        unsigned char Index;
        unsigned char Rotation;
    } Information;

    std::vector<Information> TextureInfo;
    std::vector<glm::vec3> HeightMap;
    std::vector<glm::vec3> Normals;

    std::vector<std::vector<Information> > UndoTextures;
    std::vector<std::vector<glm::vec3> > UndoTerrain;

    std::vector<unsigned char> HeightmapData;

    const unsigned int IndexAt(int X, int Z);

public:

    CTerrain(void);
    ~CTerrain(void);

    bool Load(CPodPool &PodPool, std::string Filename, std::string TexturesInfo);

    void CalculateNormals(void);

    int GetDimensions(void)
    {
        return Dimensions;
    }

    const float &GetMinHeight(void) { return MinHeight; }
    const float &GetMaxHeight(void) { return MaxHeight; }

    const unsigned int GetTextureIndex(int X, int Z);
    void GetTextureRotation(int X, int Z, int &MirrorX, int &MirrorY, int &Rotation);

    glm::vec3 GetHeight(int X, int Z);

    const glm::vec3 &GetNormal(int X, int Z);

    const unsigned char &GetHeightData(void)
    {
        return HeightmapData[0];
    }

};

}

#endif


//
//#ifndef TERRAIN_HPP
//#define TERRAIN_HPP
//
//#include <vector>
//#include <string>
//
//#include "Pods.hpp"
//
//namespace Game
//{
//
//class CTerrain
//{
//
//private:
//
//    int Dimensions;
//    float MinHeight;
//
//    typedef struct {
//        unsigned char Index;
//        unsigned char Rotation;
//    } Information;
//
//    std::vector<Information> TextureInfo;
//    std::vector<glm::vec3> HeightMap;
//    std::vector<glm::vec3> Normals;
//
//    std::vector<unsigned char> HeightmapData;
//
//    void CalculateNormals(void);
//
//    const unsigned int IndexAt(int X, int Z);
//
//public:
//
//    CTerrain(void);
//    ~CTerrain(void);
//
//    bool Load(CPodPool &PodPool, std::string Filename, std::string Info);
//
//    const int &GetDimensions(void)
//    {
//        return Dimensions;
//    }
//
//    const float &GetMinHeight(void) { return MinHeight; }
//    const unsigned int GetTextureIndex(int X, int Z);
//    void GetTextureRotation(int X, int Z, int &MirrorX, int &MirrorY, int &Rotation);
//    glm::vec3 GetHeight(int X, int Z);
//    const glm::vec3 &GetNormal(int X, int Z);
//
//    const unsigned char &GetHeightData(void)
//    {
//        return HeightmapData[0];
//    }
//
//};
//
//}
//
//#endif
