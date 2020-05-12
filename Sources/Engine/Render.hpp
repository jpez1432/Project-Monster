

#ifndef RENDER_HPP
#define RENDER_HPP

#include <gtc/constants.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_access.hpp>

#include "../Interface/imgui.h"
#include "../Interface/imgui_impl_glfw.h"
#include "../Interface/imgui_impl_opengl2.h"

#include "Input.hpp"
#include "QuadTree.hpp"
#include "Camera.hpp"
#include "Frustum.hpp"
#include "Timer.hpp"
#include "../Config.hpp"
#include "../Game/Track.hpp"
#include "../Game/Trucks.hpp"
#include "../Physics/Server.hpp"
#include "../Physics/Client.hpp"
#include "../Physics/Generic.hpp"
#include "../Physics/Vehicle.hpp"

extern std::vector<Physics::CGeneric> Objects;
extern std::vector<Physics::CGeneric> Boxes;

namespace Engine
{

class CRender
{

private:

    int Width, Height;
    GLuint PerspectiveView;
    GLuint OrthographicalView;

    glm::vec4 LightPosition;

    Physics::GeomData Data;
    Physics::CVehicle *Vehicle;

    Game::CTrack *Track;
    Game::CPodPool *PodPool;
    Game::CWeather Weather;
    Game::CTexture LogoTexture;

    Engine::CTimer *Timer;
    Engine::CQuadTree QuadTree;
    Engine::CCamera Camera;
    Engine::CInput Input;
    Engine::CFrustum Frustum;

    std::string InterfaceFile;

public:

    CRender(void);
    ~CRender(void);

    bool Create(Game::CPodPool &PodPool, Game::CTrack &Track, Physics::CVehicle &Vehicle, Engine::CTimer &Timer, std::string &InterfaceFile);

    void Scene(void);
    void Pass(float Delta, bool Invert);

    void Editor(void);
    void Interface(void);
    void Logo(void);

    void Shadows(int Index, glm::vec4 Position);
    void Reflections(void);
    void Backdrop(bool Invert);
    void Models(bool Invert);
    void Water(float Delta);
    void Terrain(CNode *Node, glm::vec3 Offset, bool Invert);

    void Resize(void);

};

}

#endif
