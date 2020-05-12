
#ifndef SERVER_HPP
#define SERVER_HPP

#define dDOUBLE

#include <Ode/Ode.h>
#include <list>

#include "../Game/Terrain.hpp"
#include "Generic.hpp"
#include "Vehicle.hpp"
#include "Client.hpp"

namespace Physics
{

class CServer
{

public:

    struct {
        float MassScale;
        float Gravity;
        float ERP;
        float CFM;
        float Slip;
        bool QuickStep;
        bool VariableStep;
        float StepSize;
        int NumIterations;
    } World;

    struct {
        bool AutoDisableFlag;
        float AutoDisableLinearThreshold;
        float AutoDisableAngularThreshold;
        int AutoDisableSteps;
        float AutoDisableTime;
        float Bounce;
        float BounceVelocity;
        float SoftERP;
        float SoftCFM;
    } Collisions;

    struct {
        float LinearDamping;
        float AngularDamping;
        float LinearDampingThreshold;
        float AngularDampingThreshold;
    } Damping;

    struct {
        int NumContacts;
        float MaxCorrectingVelocity;
        float SurfaceLayer;
    } Contacts;

public:

    dWorldID ODEWorld;
    dSpaceID Space;
    dSpaceID EnvSpace;
    dJointGroupID ContactGroup;
    dGeomID HeightField;
    dHeightfieldDataID HeightID;
    dGeomID Plane;
    GeomData EnvData;

    Game::CTerrain *Terrain;
    std::list<CClient*> ClientList;
    static CServer Instance;

    void CheckCollisions(void *Data, dGeomID o1, dGeomID o2);
    static void CollisionCallback(void *Data, dGeomID o1, dGeomID o2);

public:

    CServer(void);
    virtual ~CServer(void);

    bool LoadPhysics(std::string Filename);
    bool SavePhysics(std::string Filename);

    bool Create(Game::CTerrain *Terrain);
    bool SetTerrain(Game::CTerrain *Terrain);
    bool Destroy(void);

    bool Defaults(std::string Filename = "Default.world");

    void SetPhysics(void);

    bool Update(dReal StepSize);

    bool RegisterClient(CClient *Client);
    bool UnregisterClient(CClient *Client);
    void UnregisterAllClients(void) {
        ClientList.clear();
    }

    static CServer *GetInstance()
    {
        return &Instance;
    }


};
}


#endif
