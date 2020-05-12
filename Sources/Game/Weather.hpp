
#ifndef WEATHER_HPP
#define WEATHER_HPP

#include "Models.hpp"

namespace Game
{

class CWeather
{

private:

    glm::vec4 GeneralTint;
    glm::vec4 BackdropTint;
    glm::vec4 WaterTint;
    glm::vec4 FogColor;

    std::string SkyDomeModel;
    Game::CModel *SkyDome;

public:

    CWeather(void);
    ~CWeather(void);

    void Create(CPodPool &PodPool, unsigned int Weather);

    void ApplyTint(void);
    void ApplyBackdropTint(void);
    void ApplyWaterTint(void);

    Game::CModel &GetSkyDome(void) {
        return *SkyDome;
    }
    void DrawSkyDome(void)
    {
        glCallList(SkyDome->GetModel());
    }

};

}

#endif
