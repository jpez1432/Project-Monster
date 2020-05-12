

#include "Terrain.hpp"

using namespace Game;

CTerrain::CTerrain(void) : Dimensions(0), MinHeight(16.0f), MaxHeight(0.0f)
{


}

CTerrain::~CTerrain(void)
{

    if(TextureInfo.empty() == false) {
        TextureInfo.clear();
        Logger->Text("Released Terrain Texture Index, Rotation Memory");
    }

    if(HeightmapData.empty() == false) {
        HeightmapData.clear();
        Logger->Text("Released Terrain Heightmap Memory");
    }

    if(HeightMap.empty() == false) {
        HeightMap.clear();
        Logger->Text("Released Terrain Heightmap Scaled Memory");
    }

    if(Normals.empty() == false) {
        Normals.clear();
        Logger->Text("Released Terrain Heightmap Normals Memory");
    }

}

bool CTerrain::Load(CPodPool &PodPool, std::string Filename, std::string TexturesInfo)
{

    int Size, Offset;

    Filename.insert(0, "Data\\");
    PodPool.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Size = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Size);
    HeightmapData.resize(Dimensions * Dimensions);
    HeightMap.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            HeightmapData[x + Dimensions * z] = StreamIn.get();

            if(float(HeightmapData[x + Dimensions * z] / 16.0f) < MinHeight) {
                MinHeight = float(HeightmapData[x + Dimensions * z] / 16.0f);
            }

            if(float(HeightmapData[x + Dimensions * z] / 16.0f) > MaxHeight) {
                MaxHeight = float(HeightmapData[x + Dimensions * z] / 16.0f);
            }

            HeightMap[z + Dimensions * x] = glm::vec3(float(x), float(HeightmapData[x + Dimensions * z] / 16.0f), float(z));
        }
    }

    StreamIn.close();

    CalculateNormals();

    TexturesInfo.insert(0, "Data\\");
    PodPool.FindFile(TexturesInfo, Size, Offset);

    TexturesInfo.insert(0, RootDir + ResourceDir);
    StreamIn.open(TexturesInfo.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset == 0) {

        StreamIn.seekg(0, std::ios::end);
        Size = StreamIn.tellg();
        StreamIn.seekg(0, std::ios::beg);

    } else {

        StreamIn.seekg(Offset, std::ios::beg);

    }

    Dimensions = sqrt(Size / 2);
    TextureInfo.resize(Dimensions * Dimensions);

    for(int x = 0; x < Dimensions; x++) {
        for(int z = 0; z < Dimensions; z++) {
            TextureInfo[z + Dimensions * x].Index = StreamIn.get();
            TextureInfo[z + Dimensions * x].Rotation = StreamIn.get();
        }
    }

    StreamIn.close();

    return true;

}

void CTerrain::CalculateNormals(void)
{

    Normals.resize(HeightMap.size());
    glm::vec3 TriNormals[2][Normals.size()];

    for(int z = 0; z < Dimensions; z++) {
        for( int x = 0; x < Dimensions; x++) {

            glm::vec3 Triangle0[] = { GetHeight(x, z), GetHeight((x + 1), z), GetHeight((x + 1), (z + 1)) };
            glm::vec3 Triangle1[] = { GetHeight((x + 1), (z + 1)), GetHeight(x, (z + 1)), GetHeight(x, z) };

            glm::vec3 Normal0 = glm::cross(Triangle0[0] - Triangle0[1], Triangle0[1] - Triangle0[2]);
            glm::vec3 Normal1 = glm::cross(Triangle1[0] - Triangle1[1], Triangle1[1] - Triangle1[2]);

            TriNormals[0][z + Dimensions * x] = glm::normalize(Normal0);
            TriNormals[1][z + Dimensions * x] = glm::normalize(Normal1);

        }
    }

    glm::vec3 Result(0.0f, 0.0f, 0.0f);

    for(int z = 0; z < Dimensions; z++) {
        for(int x = 0; x < Dimensions; x++) {

            Result = glm::vec3(0);

            if(z != 0 && x != 0) {
                for(int i = 0; i < 2; i++) {
                    Result += TriNormals[i][(x - 1) + Dimensions * (z - 1)];
                }
            }

            if(x != 0 && z != Dimensions - 1) {
                Result += TriNormals[0][(x - 1) + Dimensions * z];
            }

            if(x != Dimensions - 1 && z != Dimensions - 1) {
                for(int i = 0; i < 2; i++) {
                    Result += TriNormals[i][x + Dimensions * z];
                }
            }

            if(x != Dimensions - 1 && z != 0) {
                Result += TriNormals[1][x + Dimensions * (z - 1)];
            }

            Normals[x + Dimensions * z] = glm::normalize(Result);

        }
    }

}

const unsigned int CTerrain::IndexAt(int X, int Z)
{
    X = glm::clamp(X, 0, int(Dimensions));
    Z = glm::clamp(Z, 0, int(Dimensions));

    return int(((Z + Dimensions) % Dimensions) + ((X + Dimensions) % Dimensions) * Dimensions);
}

const unsigned int CTerrain::GetTextureIndex(int X, int Z)
{
    return TextureInfo[IndexAt(X, Z)].Index + ((TextureInfo[IndexAt(X, Z)].Rotation & 1) << 8);
}

void CTerrain::GetTextureRotation(int X, int Z, int &MirrorX, int &MirrorY, int &Rotation)
{
    MirrorX = (TextureInfo[IndexAt(X, Z)].Rotation & 32) >> 5;
    MirrorY = (TextureInfo[IndexAt(X, Z)].Rotation & 16) >> 4;
    Rotation = (TextureInfo[IndexAt(X, Z)].Rotation & 192) >> 6;
}

glm::vec3 CTerrain::GetHeight(int X, int Z)
{
    return glm::vec3(X, HeightMap[IndexAt(X, Z)].y, Z);
}

const glm::vec3 &CTerrain::GetNormal(int X, int Z)
{

    return Normals[IndexAt(X, Z)];
}

