
#ifndef MAP_HPP
#define MAP_HPP

#include <algorithm>

#include "../Common.hpp"
#include "Pods.hpp"
#include "Terrain.hpp"
#include "Textures.hpp"
#include "Boxes.hpp"
#include "Models.hpp"
#include "Weather.hpp"

namespace Game
{

class CMap
{

private:

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
        int Bounds;
        GLuint GLModel;
    } Model;

    std::string Level;
    std::string Name;
    std::string Locale;
    std::string Logo;
    std::string Description;
    std::string Type;
    std::string Backdrop;
    std::string WeatherBackdrop;

    std::string TextFile;
    std::string HeightMap;
    std::string TextureInfo;
    std::string Palette;
    std::string TextureList;
    std::string LightMap;

    std::vector<Vehicle> Vehicles;
    std::vector<Model> Models;

    glm::vec3 SunPosition;

    float WaterHeight;
    GLuint GLWater;
    Game::CTexture WaterTexture;

    Game::CModel Background;

    Game::CTexturePool *TexturePool;
    Game::CModelPool *ModelPool;
    Game::CTerrain *Terrain;
    Game::CBoxes *Boxes;

    std::string FindMarker(std::ifstream &Stream, std::string Marker1, std::string Marker2 = "");

public:

    CMap(void);
    ~CMap(void);

    bool Load(CPodPool &PodPool, std::string Filename);
    bool Destroy(void);


    CTexturePool *TexturePoolObject(void)
    {
        return TexturePool;
    }

    void DrawModel(int Index)
    {
        glCallList(Models[Index].GLModel);
    }

    const int NumModels(void)
    {
        return Models.size();
    }

    CModelPool *ModelPoolObject(void)
    {
        return ModelPool;
    }

    const glm::vec3 GetModelPosition(int Index)
    {
        return Models[Index].Position / 32.0f;
    }
    const glm::vec3 GetModelRotation(int Index)
    {
        return Models[Index].Rotation;
    }
    const std::string GetModelTypeFlags(int Index)
    {
        return Models[Index].TypeFlags;
    }
    const int GetModelPriority(int Index)
    {
        return Models[Index].Priority;
    }
    const float GetModelBounds(int Index)
    {
        return Models[Index].Bounds;
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
    void DrawBackground(void)
    {
        glCallList(Background.GetModel());
    }

    const glm::vec4 GetSunPosition(void)
    {
        return glm::vec4(SunPosition, 1.0f);
    }

    const float GetWaterHeight(void)
    {
        return WaterHeight;
    }

    Game::CTexture *GetWaterTexture(void)
    {
        return &WaterTexture;
    }

    const glm::vec3 GetStartPosition(int Index)
    {
        return Vehicles[Index].Position / 32.0f;
    }
    const glm::vec3 GetStartRotation(int Index)
    {
        return Vehicles[Index].Rotation;
    }



};

}

#endif
