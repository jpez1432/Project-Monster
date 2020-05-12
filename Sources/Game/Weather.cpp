
#include "Weather.hpp"

using namespace Game;

CWeather::CWeather(void) : SkyDome(NULL)
{

}

CWeather::~CWeather(void)
{

}

void CWeather::Create(CPodPool &PodPool, unsigned int Weather)
{
    glm::vec4 FogColor(0.0f);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, &FogColor[0]);
    glHint(GL_FOG_HINT, GL_NICEST);
    glFogf(GL_FOG_END, Config->Graphics.FarClip);
    glDisable(GL_FOG);

    switch(Weather) {

    case 0:	// Clear SkyDome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 1:	// Cloudy SkyDome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 0.8f;
        GeneralTint[1] = 0.8f;
        GeneralTint[2] = 0.8f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 2:	// Fog Skydome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        glFogf(GL_FOG_DENSITY, 0.125f);
        glEnable(GL_FOG);
        break;

    case 3:	// Dense Fog Skydome
        SkyDomeModel = "CLOUDY2.BIN";
        GeneralTint[0] = 1.0f;
        GeneralTint[1] = 1.0f;
        GeneralTint[2] = 1.0f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        glFogf(GL_FOG_DENSITY, 0.25f);
        glEnable(GL_FOG);
        break;

    case 4:	// Rainy SkyDome
        SkyDomeModel = "CCLOUDS.BIN";
        GeneralTint[0] = 0.5f;
        GeneralTint[1] = 0.5f;
        GeneralTint[2] = 0.5f;
        BackdropTint[0] = 0.3f;
        BackdropTint[1] = 0.3f;
        BackdropTint[2] = 0.3f;
        break;

    case 5:	// Dusk SkyDome
        SkyDomeModel = "DUSKSKY.BIN";
        GeneralTint[0] = 0.8f;
        GeneralTint[1] = 0.7f;
        GeneralTint[2] = 0.6f;
        BackdropTint[0] = 0.3f;
        BackdropTint[1] = 0.3f;
        BackdropTint[2] = 0.3f;
        break;

    case 6:	// Night SkyDome
        SkyDomeModel = "NITESKY.BIN";
        GeneralTint[0] = 0.3f;
        GeneralTint[1] = 0.3f;
        GeneralTint[2] = 0.3f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;

    case 7:	// Night SkyDome (Pitch Black)
        SkyDomeModel = "NITESKY.BIN";
        GeneralTint[0] = 0.1f;
        GeneralTint[1] = 0.1f;
        GeneralTint[2] = 0.1f;
        BackdropTint[0] = GeneralTint[0];
        BackdropTint[1] = GeneralTint[1];
        BackdropTint[2] = GeneralTint[2];
        break;
    }

    WaterTint[0] = 0.6f;
    WaterTint[1] = 0.8f;
    WaterTint[2] = 1.0f;
    WaterTint[3] = 0.2f;

    GeneralTint[3] = BackdropTint[3] = 1.0f;

    if (SkyDome) {
        delete SkyDome;
    }

    SkyDome = new Game::CModel();
    if(SkyDome->Load(PodPool, SkyDomeModel)) {
        Logger->Text("Loaded Weather SkyDome Model - " + SkyDomeModel);
    } else {
        Logger->Error("Error Loading Weather SkyDome Model - " + SkyDomeModel);
    }
}

void CWeather::ApplyTint(void)
{

    glLightfv(GL_LIGHT0, GL_AMBIENT, &GeneralTint[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, &GeneralTint[0]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, &glm::vec4(1.0f)[0]);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    if (Config->Render.Lighted) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }

}

void CWeather::ApplyBackdropTint(void)
{
    glDisable(GL_LIGHTING);
    glColor4f(BackdropTint.r, BackdropTint.g, BackdropTint.b, BackdropTint.a);

}

void CWeather::ApplyWaterTint(void)
{
    glEnable(GL_BLEND);
    glDisable(GL_LIGHTING);
    glColor4f(WaterTint.r, WaterTint.g, WaterTint.b, WaterTint.a);

}
