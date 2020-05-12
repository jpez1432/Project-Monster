
#define GLEW_STATIC

#include <GL/Glew.h>

#include "Config.hpp"
#include "Engine/Render.hpp"
#include "Engine/Timer.hpp"
#include "Engine/Input.hpp"
#include "Physics/Server.hpp"

bool Loop = true;
std::string ResourceDir;
std::string ConfigFile;
std::string InterfaceFile;
std::string RootDir;

CConfig *Config;
CLogger *Logger;

Game::CPodPool *PodPool;
Game::CTrack *Track;

Physics::GeomData Data;
std::vector<Physics::CGeneric> Objects;
std::vector<Physics::CGeneric> Boxes;
Physics::CVehicle *Vehicle;

Engine::CInput *Input;
Engine::CTimer Timer;
Engine::CRender *Render;
GLFWwindow *GLWindow;

void ErrorCallback(int Error, const char* Description)
{
    Logger->Error("GLFW3 Error #" + std::to_string(Error) + ", " + std::string(Description));
}

void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
    ImGui_ImplGlfw_KeyCallback(Window, Key, Scancode, Action, Mods);

    if(Key == GLFW_KEY_H && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.HelpMode = !Config->General.HelpMode;

    } else if(Key == GLFW_KEY_E && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.EditMode = !Config->General.EditMode;

    } else if(Key == GLFW_KEY_D && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.DebugMode = !Config->General.DebugMode;

    } else if(Key == GLFW_KEY_P && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.PhysicsMode = !Config->General.PhysicsMode;

    } else if(Key == GLFW_KEY_F && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->General.FpsCounter = !Config->General.FpsCounter;

    } else if(Key == GLFW_KEY_W && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->Render.Wrap = !Config->Render.Wrap;

    } else if(Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS) {

        Config->General.Paused = !Config->General.Paused;

        if (Config->General.Paused && !Config->General.PhysicsMode) {
            glfwSetInputMode(GLWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        } else {
            glfwSetInputMode(GLWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

    } else if(Key == GLFW_KEY_Q && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        glfwSetWindowShouldClose(GLWindow, GLFW_TRUE);

    } else if(Key == GLFW_KEY_R && Action == GLFW_RELEASE) {

        Vehicle->Reset();

    } else if(Key == GLFW_KEY_F && Action == GLFW_RELEASE) {

        Vehicle->Flip();

    } else if(Key == GLFW_KEY_C && Action == GLFW_PRESS) {

        Config->Camera.Mode++;

        if(Config->Camera.Mode > 4) {
            Config->Camera.Mode = 0;
        }

    } else if(Key == GLFW_KEY_V && Mods == GLFW_MOD_CONTROL && Action == GLFW_PRESS) {

        Config->Graphics.VSync = !Config->Graphics.VSync;
        glfwSwapInterval(Config->Graphics.VSync);

    } else if(Key == GLFW_KEY_ENTER && Mods == GLFW_MOD_ALT && Action == GLFW_PRESS) {

        Config->Graphics.Fullscreen = !Config->Graphics.Fullscreen;

        int Count, PosX, PosY;
        GLFWmonitor** Monitors = glfwGetMonitors(&Count);

        if(Config->Graphics.Fullscreen) {
            glfwSetWindowMonitor(GLWindow, Monitors[int(Config->Graphics.Monitor)], 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
        } else {

//            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
            const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor)]);
            glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor)], &PosX, &PosY);
            glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
        }
        glfwSwapInterval(Config->Graphics.VSync);
    }


    if(Config->General.EditMode) {

        if(Key == GLFW_KEY_F1 && Action == GLFW_PRESS) {

            Config->Render.Culled = !Config->Render.Culled;

        } else if(Key == GLFW_KEY_F2 && Action == GLFW_PRESS) {

            Config->Render.Lighted = !Config->Render.Lighted;

        } else if(Key == GLFW_KEY_F3 && Action == GLFW_PRESS) {

            Config->Render.Textured = !Config->Render.Textured;

        } else if(Key == GLFW_KEY_F4 && Action == GLFW_PRESS) {

            Config->Render.Wireframe = !Config->Render.Wireframe;

        } else if(Key == GLFW_KEY_F5 && Action == GLFW_PRESS) {

            Config->Render.Terrain = !Config->Render.Terrain;

        } else if(Key == GLFW_KEY_F6 && Action == GLFW_PRESS) {

            Config->Render.Boxes = !Config->Render.Boxes;

        } else if(Key == GLFW_KEY_F7 && Action == GLFW_PRESS) {

            Config->Render.Models = !Config->Render.Models;

        } else if(Key == GLFW_KEY_F8 && Action == GLFW_PRESS) {

            Config->Render.Water = !Config->Render.Water;

            if(Track->GetWaterHeight() == -1.0f) {
                Config->Render.Water = false;
                Config->Render.Reflections = false;
            }

        } else if(Key == GLFW_KEY_F9 && Action == GLFW_PRESS) {

            Config->Render.Backdrop = !Config->Render.Backdrop;

        } else if(Key == GLFW_KEY_F10 && Action == GLFW_PRESS) {

            Config->Render.Reflections = !Config->Render.Reflections;

            if(Track->GetWaterHeight() == -1.0f) {
                Config->Render.Water = false;
                Config->Render.Reflections = false;
            }

        } else if(Key == GLFW_KEY_F11 && Action == GLFW_PRESS) {

            Config->Render.Shadows = !Config->Render.Shadows;
        }
    }

}

void MouseScrollCallback(GLFWwindow* Window, double xOffset, double yOffset)
{

    float Scale = 0.15f;
    if(glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        Scale *= 5.0f;
    }

    Config->Camera.Radius += yOffset * Scale;
    Config->Camera.Radius = glm::clamp(Config->Camera.Radius, 1.0f, 10.0f);

}

void ResizeCallback(GLFWwindow* Window, int Width, int Height)
{
    glfwGetFramebufferSize(GLWindow, &Config->Graphics.Width, &Config->Graphics.Height);
    Config->Graphics.AspectRatio = float(Config->Graphics.Width / Config->Graphics.Height);
    Render->Resize();
}

bool Create(std::string ResDir, std::string CfgFile, std::string AppRoot)
{

    RootDir = AppRoot.substr(0, AppRoot.find_last_of("\\") + 1);
    ResourceDir = ResDir;
    InterfaceFile = RootDir + ResourceDir + "Monster.gui";
    ConfigFile = RootDir + ResourceDir + CfgFile;

    Logger = new CLogger(RootDir + ResourceDir + "Monster.log");
    Config = new CConfig(ConfigFile);

    Logger->Text(GetInfo());
    Logger->Text("Resource Directory - " + ResDir);
    Logger->Text("Configuration Filename - " + CfgFile);
    Logger->Text("Interface Filename - Interface.cfg");
    Logger->Text("Root Directory - " + RootDir);
    Logger->Text("Pod.Ini Filename - " + std::string(Config->Game.PodIni));

    glfwSetErrorCallback(ErrorCallback);

    if(glfwInit()) {
        Logger->Text("Initialized GLFW3 Library");
    } else {
        Logger->Error("Error Initializing GLFW3 Library");
    }

    glfwWindowHint(GLFW_SAMPLES, Config->Graphics.Antialiasing);
    glfwWindowHint(GLFW_DOUBLEBUFFER, Config->Graphics.DoubleBuffer);
    glfwWindowHint(GLFW_REFRESH_RATE, Config->Graphics.RefreshRate);

    int Cnt, PosX, PosY;
    GLFWmonitor** Monitors = glfwGetMonitors(&Cnt);

    if(Config->Graphics.Monitor > Cnt) {
        Logger->Error("Invalid Monitor Selection, Defaulting To Primary...", true, true, false);
        Config->Graphics.Monitor = 0;
    }

    if(Config->Graphics.Fullscreen) {
        GLWindow = glfwCreateWindow(Config->Graphics.Width, Config->Graphics.Height, GetInfo().c_str(), Monitors[int(Config->Graphics.Monitor)], NULL);
    } else {

//        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        GLWindow = glfwCreateWindow(Config->Graphics.Width, Config->Graphics.Height, GetInfo().c_str(), NULL, NULL);
        const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor)]);
        glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor)], &PosX, &PosY);
        glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));
    }

    if(GLWindow) {
        Logger->Text("Created GLFW3 Window");
    } else {
        Logger->Error("Error Creating GLFW3 Window");
    }

//    GLFWimage Icon;
//    std::string IconFile = RootDir + ResDir + "//Game//Icon.png";
//    Icon.pixels = SOIL_load_image(IconFile.c_str(), &Icon.width, &Icon.height, 0, SOIL_LOAD_RGBA);
//    glfwSetWindowIcon(GLWindow, 1, &Icon);
//    SOIL_free_image_data(Icon.pixels);

    glfwMakeContextCurrent(GLWindow);
    glfwSwapInterval(Config->Graphics.VSync);

    if(Config->Graphics.Multisample) {
        glEnable(GL_MULTISAMPLE);
    }

    if(GLEW_OK == glewInit()) {
        Logger->Text("Initialized GLEW");
    } else {
        Logger->Error("Error Initializing GLEW");
    }

    Logger->Text("GLEW - v" + std::string((char*)glewGetString(GLEW_VERSION)));
    Logger->Text("GLFW3 - v" + std::string((char*)glfwGetVersionString()));
    Logger->Text("OpenGL Renderer - " + std::string((char*)glGetString(GL_RENDERER)));
    Logger->Text("OpenGL Vendor - " + std::string((char*)glGetString(GL_VENDOR)));
    Logger->Text("OpenGL Version - v" + std::string((char*)glGetString(GL_VERSION)));

    PodPool = new Game::CPodPool();
    if(PodPool->Create(Config->Game.PodIni)) {
        Logger->Text("Loaded Pod Information - " + std::string(Config->Game.PodIni));
    } else {
        Logger->Error("Error Loading Pod Information - " + std::string(Config->Game.PodIni));
    }

    if(PodPool->Prioritize("World\\" + std::string(Config->Game.Track), 1)) {
        Logger->Text("Prioritized Track Pod File - " + std::string(Config->Game.Track));
    } else {
        Logger->Error("Couldn't Prioritize Track Pod File - " + std::string(Config->Game.Track), true, false, false);
    }

    Track = new Game::CTrack();
    if(Track->Load(*PodPool, Config->Game.Track)) {
        Logger->Text("Loaded Track - " + std::string(Config->Game.Track));
    } else {
        Logger->Error("Error Loading  Track - " + std::string(Config->Game.Track) + ", Defaulting...", true, false, false);
        Track->Load(*PodPool, &PodPool->TrackFilename(0));
    }

    if(Physics::CServer::GetInstance()->LoadPhysics(Config->Physics.WorldFile)) {
        Logger->Text("Loaded World Physics");
    } else {
        Logger->Error("Error Loading World Physics");
    }

    if(Physics::CServer::GetInstance()->Create(Track->TerrainObject())) {
        Logger->Text("Created Physics Server");
    } else {
        Logger->Error("Error Creating Physics Server");
    }

    Data.Slip = 0.000001f;
    Game::CModel Dummy;

    Objects.resize(Track->Models.size());
    for(unsigned int i = 0; i < Track->Models.size(); i++) {

        if((Track->Models[i].Filename != "Box") && Track->GetModelTypeFlags(i) != "6,0" && Track->GetModelTypeFlags(i) != "8,0") {

            Physics::CServer::GetInstance()->RegisterClient(&Objects[i]);
            Objects[i].Create(dTriMeshClass, Track->GetModel(i), Track->Models[i].Mass > 0.0f ? false : true, Track->Models[i].TypeFlags[0]);
            Objects[i].SetMass(Track->Models[i].Mass);
            Objects[i].SetData(&Data);
            Objects[i].SetRotation(Track->Models[i].Rotation.x, Track->Models[i].Rotation.y, Track->Models[i].Rotation.z);
            Objects[i].SetPosition(Track->Models[i].Position.x, Track->Models[i].Position.y, Track->Models[i].Position.z);
            Objects[i].SetEnviroment();

        } else {

            Physics::CServer::GetInstance()->RegisterClient(&Objects[i]);
            Objects[i].Create(dBoxClass, Dummy, true, Track->Models[i].TypeFlags[0], Track->Models[i].Dimensions.y, Track->Models[i].Dimensions.x, Track->Models[i].Dimensions.z, 0, 0);
            Objects[i].SetMass(1.0f);
            Objects[i].SetData(&Data);
            Objects[i].SetRotation(Track->Models[i].Rotation.x, Track->Models[i].Rotation.y, Track->Models[i].Rotation.z);
            Objects[i].SetPosition(Track->Models[i].Position.x, Track->Models[i].Position.y, Track->Models[i].Position.z);
            Objects[i].SetEnviroment();

        }
    }

    int Count = 0;
    float Lower, Upper;

    for(int x = 0; x < Track->Dimensions(); x++) {
        for(int z = 0; z < Track->Dimensions(); z++) {
            if(Track->Boxes->GetLowerHeight(x, z) != Track->Boxes->GetUpperHeight(x, z)) {
                Count++;
            }
        }
    }

    Boxes.resize(Count);
    Count = 0;
    for(int x = 0; x < Track->Dimensions(); x++) {
        for( int z = 0; z < Track->Dimensions(); z++) {

            Lower = Track->Boxes->GetLowerHeight(x, z);
            Upper = Track->Boxes->GetUpperHeight(x, z);

            if(Upper != Lower) {
                Physics::CServer::GetInstance()->RegisterClient(&Boxes[Count]);
                Boxes[Count].Create(dBoxClass, Dummy, true, 0, 1.0f, (Upper - Lower), 1.0f, 0.0f, 0);
                Boxes[Count].SetMass(0);
                Boxes[Count].SetData(&Data);
                Boxes[Count].SetPosition(x + 0.5f, Lower + (Upper - Lower) / 2, z + 0.5f);
                Boxes[Count].SetEnviroment();
                Count++;
            }
        }
    }

    Vehicle = new Physics::CVehicle();
    Physics::CServer::GetInstance()->RegisterClient(Vehicle);

    if(Vehicle->LoadTruck(*PodPool, Config->Game.Vehicle)) {
        Logger->Text("Loaded Truck - " + std::string(Config->Game.Vehicle));
    } else {
        Logger->Error("Error Loading Truck - " + std::string(Config->Game.Vehicle) + ", Defaulting...", true, false, false);
        Vehicle->LoadTruck(*PodPool, &PodPool->TruckFilename(0));
    }

    if(Vehicle->LoadPhysics(Config->Physics.VehicleFile)) {
        Logger->Text("Loaded Vehicle Physics");
    } else {
        Logger->Error("Error Loading Vehicle Physics");
    }

    if(Vehicle->Create(Track->GetStartPosition(0), Track->GetStartRotation(0), Track->Dimensions())) {
        Logger->Text("Created ODE Physics Vehicle");
    } else {
        Logger->Error("Error Creating ODE Physics Vehicle");
    }

    if(Config->Input.Joypad) {
        Input = new Engine::CInput();
        if (Input->Create(GLFW_JOYSTICK_1)) {
            if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
                Logger->Text("Gamepad Found - " + std::string(glfwGetJoystickName(GLFW_JOYSTICK_1)));
            } else {
                Logger->Text("Joystick Found - " + std::string(glfwGetJoystickName(GLFW_JOYSTICK_1)));
            }
        } else {
            Logger->Error("No Joypad/Gamepad Found...", false, false, false);
//            Config->Input.Joypad = false;
        }
    }

    Render = new Engine::CRender();
    if(Render->Create(*PodPool, *Track, *Vehicle, Timer, InterfaceFile)) {
        Logger->Text("Created Rendering Engine");
    } else {
        Logger->Error("Error Creating Rendering Engine");
    }

    glfwSetKeyCallback(GLWindow, (GLFWkeyfun)KeyCallback);
    glfwSetWindowSizeCallback(GLWindow, ResizeCallback);
    glfwSetInputMode(GLWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetScrollCallback(GLWindow, MouseScrollCallback);

    glfwSetInputMode(GLWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    Logger->Text("Project Monster Created");

    return true;

}

bool Destroy(void)
{

    if(Render) {
        delete Render;
        Render = NULL;
        Logger->Text("Destroyed Rendering Object");
    }

    if(Config->Physics.Save) {
        Physics::CServer::GetInstance()->SavePhysics(Config->Physics.WorldFile);
        Logger->Text("Save World Physics");
    }

    for(unsigned int i = 0; i < Objects.size(); i++) {
        Physics::CServer::GetInstance()->UnregisterClient(&Objects[i]);
    }

    Objects.clear();
    Logger->Text("Unregistered Objects Client From Server");

    for(unsigned int i = 0; i < Boxes.size(); i++) {
        Physics::CServer::GetInstance()->UnregisterClient(&Boxes[i]);
    }

    Boxes.clear();
    Logger->Text("Unregistered Boxes Client From Server");

    if(Physics::CServer::GetInstance()->Destroy()) {
        Logger->Text("Destroyed Physics Server Object");
    } else {
        Logger->Error("Error Destroying Physics Server Object");
    }


    if(Config) {
        Config->Save(ConfigFile);
        delete Config;
        Config = NULL;
        Logger->Text("Destroyed Configuration Object");
    }

    if(GLWindow) {
        glfwTerminate();
        Logger->Text("Destroyed OpenGL Framework 3 Window");
    }

    Logger->Text("Project Monster Destroyed");

    if(Logger) {
        delete Logger;
        Logger = NULL;
    }

    return true;

}

int main(int argc, char* argv[])
{
    while(Loop) {

//        Loop = Create("Resources\\", "Monster.cfg", argv[0]);
        Loop = Create("..\\Resources\\", "Monster.cfg", argv[0]);

        do {

            glfwPollEvents();
            Input->Update();

            if (!Config->General.Paused) {
                Physics::CServer::GetInstance()->Update(Timer.Update());
            }

            Render->Scene();

        } while(Loop && !glfwWindowShouldClose(GLWindow));

        Destroy();
        Loop = !Loop;

    };

    return EXIT_SUCCESS;

}
