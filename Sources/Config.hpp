
#ifndef CCONFIG_HPP
#define CCONFIG_HPP

#include <Glm.hpp>

#include "Logger.hpp"

extern CLogger *Logger;
extern std::string ConfigFile;

class CConfig
{

private:

public:

    struct {
        bool Paused;
        bool HelpMode;
        bool EditMode;
        bool DebugMode;
        bool PhysicsMode;
        bool FpsCounter;
    } General;

    struct {
        bool Save;
        bool Defaults;
        char WorldFile[32];
        char VehicleFile[32];
    } Physics;

    struct {
        bool Pods;
        bool Files;
        char PodIni[32];
        char Track[32];
        char Vehicle[32];
        unsigned char Weather;
        unsigned char Scenery;
    } Game;

    struct {
        bool Wireframe;
        bool Textured;
        bool Lighted;
        bool Culled;
        bool Reflections;
        bool Terrain;
        bool Boxes;
        bool Models;
        bool Water;
        bool Backdrop;
        bool Shadows;
        bool Wrap;
    } Render;

    struct {
        int Width;
        int Height;
        char Monitor;
        char RefreshRate;
        char Antialiasing;
        float FieldOfVision;
        float AspectRatio;
        float NearClip;
        float FarClip;
        bool VSync;
        bool DoubleBuffer;
        bool Fullscreen;
        bool Multisample;
        char TextureQuality;
    } Graphics;

    struct {
        bool Invert;
        char Mode;
        glm::vec3 Position;
        glm::vec3 Center;
        glm::vec3 Up;
        float Yaw;
        float Pitch;
        float Radius;
        float Angle;
    } Camera;

    struct {
        bool Joypad;
        bool MouseLook;
        float MouseSensitivity;
        float MovementSpeed;
        int Gas;
        int Brake;
        int ShiftUp;
        int ShiftDown;
        int UpShift;
        int DownShift;
        int FrontSteerLeft;
        int FrontSteerRight;
        int RearSteerLeft;
        int RearSteerRight;
        int Forwards;
        int Backwards;
        int StrafeLeft;
        int StrafeRight;
        int Up;
        int Down;
    } Input;

    struct {
        int Flip;
        int Reset;
        int Gas;
        int Brake;
        int UpShift;
        int DownShift;
        int FrontSteerLeft;
        int FrontSteerRight;
        int RearSteerLeft;
        int RearSteerRight;
    } Joypad;

    void Defaults(std::string Filename = "");

public:

    CConfig(void);
    CConfig(std::string Filename);
    ~CConfig(void);

    bool Load(std::string Filename);
    bool Save(std::string Filename = "");

};

#endif
