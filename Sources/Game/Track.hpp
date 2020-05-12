
#ifndef TRACK_HPP
#define TRACK_HPP

#include "../Common.hpp"

#include "Pods.hpp"
#include "Terrain.hpp"
#include "Textures.hpp"
#include "Boxes.hpp"
#include "Models.hpp"
#include "Weather.hpp"

namespace Game
{

class CTrack
{

public:

    bool MTM2Track;

    typedef struct {
        std::string Filename;
        glm::vec3 Position;
        glm::vec3 Rotation;
    } Vehicle;

    typedef struct {
        std::string Filename;
        glm::vec3 Position;
        glm::vec3 Rotation;
        glm::vec3 Dimensions;
        float Mass;
        std::string TypeFlags;
        int Priority;
        int Index;
        bool Reflects;
    } Model;

    std::string Level;
    std::string Name;
    std::string Locale;
    std::string Logo;
    std::string Description;
    std::string Type;
    std::string WeatherBackdrop;
    std::string TextFile;
    std::string Backdrop;
    std::string HeightMap;
    std::string TextureInfo;
    std::string Palette;
    std::string TextureList;
    std::string LightMap;

    std::vector<CModel> Backdrops;
    std::vector<Vehicle> Vehicles;
    std::vector<Model> Models;

    glm::vec3 SunPosition;

    int BackdropCount;

    float WaterHeight;
    GLuint GLWater;
    Game::CTexture WaterTexture;

    Game::CTexturePool *TexturePool;
    Game::CModelPool *ModelPool;
    Game::CTerrain *Terrain;
    Game::CBoxes *Boxes;

    std::string FindMarker(std::ifstream &Stream, std::string Marker1, std::string Marker2 = "");

public:

    CTrack(void);
    ~CTrack(void);

    bool Load(CPodPool &PodPool, std::string Filename);
    void Destroy(void);

    CTexturePool *TexturePoolObject(void)
    {
        return TexturePool;
    }
    void DrawModel(int Index)
    {
        glCallList(ModelPool->GetModel(Models[Index].Index).GLModel);
    }
    const int NumModels(void)
    {
        return Models.size();
    }
    CModelPool *ModelPoolObject(void)
    {
        return ModelPool;
    }
    CModel &GetModel(int Index)
    {
        return ModelPool->GetModel(Models[Index].Index);
    }
    const bool &GetReflects(int Index)
    {
        return Models[Index].Reflects;
    }
    const glm::vec3 &GetModelPosition(int Index)
    {
        return Models[Index].Position;
    }
    const glm::vec3 &GetModelRotation(int Index)
    {
        return Models[Index].Rotation;
    }
    const std::string &GetModelTypeFlags(int Index)
    {
        return Models[Index].TypeFlags;
    }
    const int &GetModelPriority(int Index)
    {
        return Models[Index].Priority;
    }
    const float GetModelBounds(int Index, glm::vec3 &Min, glm::vec3 &Max)
    {
        Min = ModelPool->GetModel(Models[Index].Index).Min;
        Max = ModelPool->GetModel(Models[Index].Index).Max;
        return ModelPool->GetModel(Models[Index].Index).BoundingSphere;
    }

    const int Dimensions(void)
    {
        return Terrain->GetDimensions();
    }
    CTerrain *TerrainObject(void)
    {
        return Terrain;
    }

    CBoxes *BoxesObject(void)
    {
        return Boxes;
    }
    void DrawBackground(int Index)
    {
        glCallList(Backdrops[Index].GetModel());
    }
    int GetBackdropCount(void)
    {
        return Backdrops.size();
    }
    Game::CModel &GetBackdrop(int Index) {
        return Backdrops[Index];
    }

    const glm::vec4 GetSunPosition(void)
    {
//        return glm::vec4(SunPosition.y, 16.0f, SunPosition.z, 1.0f);
        return glm::vec4(SunPosition, 1.0f);
    }

    const float &GetWaterHeight(void)
    {
        return WaterHeight;
    }

    Game::CTexture *GetWaterTexture(void)
    {
        return &WaterTexture;
    }

    const glm::vec3 &GetStartPosition(int Index)
    {
        return Vehicles[Index].Position;
    }
    const glm::vec3 &GetStartRotation(int Index)
    {
        return Vehicles[Index].Rotation;
    }

};

}

#endif
