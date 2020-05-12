
#include "Server.hpp"

using namespace Physics;

CServer CServer::Instance;

CServer::CServer(void)
{
}

CServer::~CServer(void)
{
    Destroy();
}

bool CServer::Defaults(std::string Filename)
{

//    World.MassScale = 1.0f;
//    World.Gravity = -9.8f;
//    World.ERP = 0.8f;
//    World.CFM = 0.000001f;
//    World.Slip = 0.000001f;
//    World.QuickStep = false;
//    World.VariableStep = false;
//    World.StepSize = 0.01666667f;
//    World.NumIterations = 60;
//
//    Collisions.AutoDisableFlag = 1;
//    Collisions.AutoDisableLinearThreshold = 0.01f;
//    Collisions.AutoDisableAngularThreshold = 0.01f;
//    Collisions.AutoDisableSteps = 1;
//    Collisions.AutoDisableTime = 1;
//    Collisions.Bounce = 0.5;
//    Collisions.BounceVelocity = 0.5;
//    Collisions.SoftCFM = 0.000001f;
//    Collisions.SoftERP = 1.0f;
//
//    Damping.LinearDamping = 0.05f;
//    Damping.AngularDamping = 0.05f;
//    Damping.LinearDampingThreshold = 0.01f;
//    Damping.AngularDampingThreshold = 0.01f;
//
//    Contacts.NumContacts = 60;
//    Contacts.MaxCorrectingVelocity = 0.025f;
//    Contacts.SurfaceLayer = 0.0f;

//    World.MassScale = 1.0f;
//    World.Gravity = -9.8f;
//    World.ERP = 0.8f;
//    World.CFM = 0.000001f;
//    World.Slip = 0.000001f;
//    World.QuickStep = false;
//    World.VariableStep = false;
//    World.StepSize = 0.01666667f;
//    World.NumIterations = 30;
//
//    Collisions.AutoDisableFlag = 1;
//    Collisions.AutoDisableLinearThreshold = 0.01f;
//    Collisions.AutoDisableAngularThreshold = 0.01f;
//    Collisions.AutoDisableSteps = 1;
//    Collisions.AutoDisableTime = 1;
//    Collisions.Bounce = 0.5;
//    Collisions.BounceVelocity = 0.5;
//    Collisions.SoftCFM = 0.000001f;
//    Collisions.SoftERP = 1.0f;
//
//    Damping.LinearDamping = 0.05f;
//    Damping.AngularDamping = 0.05f;
//    Damping.LinearDampingThreshold = 0.01f;
//    Damping.AngularDampingThreshold = 0.01f;
//
//    Contacts.NumContacts = 24;
//    Contacts.MaxCorrectingVelocity = 0.025f;
//    Contacts.SurfaceLayer = 0.0f;

    World.MassScale = 1.0f;
    World.Gravity = -9.8f;// / 2.0f;
    World.ERP = 0.8f;
    World.CFM = 0.00000068f;
    World.Slip = 0.000001f;
    World.QuickStep = false;
    World.VariableStep = false;
    World.StepSize = 0.01666667f;
    World.NumIterations = 30;

    Collisions.AutoDisableFlag = 1;
    Collisions.AutoDisableLinearThreshold = 0.01f;
    Collisions.AutoDisableAngularThreshold = 0.01f;
    Collisions.AutoDisableSteps = 10;
    Collisions.AutoDisableTime = 0;
    Collisions.Bounce = 0.5f;
    Collisions.BounceVelocity = 0.5f;
    Collisions.SoftCFM = 0.0000001f;
    Collisions.SoftERP = 1.0f;

    Damping.LinearDamping = 0.000000001f;//0.000125f;
    Damping.AngularDamping = 0.050000001f;//0.05f;
    Damping.LinearDampingThreshold = 0.05f;//0.01000000f;//0.025f;
    Damping.AngularDampingThreshold = 0.05f;//0.010000000f;//0.5f;

    Contacts.NumContacts = 20;
    Contacts.MaxCorrectingVelocity = 0.025f;
    Contacts.SurfaceLayer = 0.0f;

//    SavePhysics(Filename);

    return true;

}

bool CServer::LoadPhysics(std::string Filename)
{
    std::string PhysicsFile = RootDir + ResourceDir + "Physics\\" + Filename;

    if(Config->Physics.Defaults)
    {
        Defaults(Filename);
        return true;
    }

    std::ifstream CfgFile(PhysicsFile.c_str(), std::ios::in | std::ios::binary);

    if (CfgFile.fail()) {
        Logger->Error("Error Loading World Physics, Loading Defaults...", true, true, false);
        Defaults(Filename);
        return true;
    }

    CfgFile.read((char*)&World, sizeof(World));
    CfgFile.read((char*)&Collisions, sizeof(Collisions));
    CfgFile.read((char*)&Damping, sizeof(Damping));
    CfgFile.read((char*)&Contacts, sizeof(Contacts));

    CfgFile.close();

    return true;
}

bool CServer::SavePhysics(std::string Filename)
{
    Filename.insert(0, RootDir + ResourceDir + "Physics\\");
    std::ofstream CfgFile(Filename.c_str(), std::ios::in | std::ios::binary | std::ios::trunc);

    if (CfgFile.fail()) {
        Logger->Error("Error Saving World Physics...", true, true, false);
        return false;
    }

    CfgFile.write((char*)&World, sizeof(World));
    CfgFile.write((char*)&Collisions, sizeof(Collisions));
    CfgFile.write((char*)&Damping, sizeof(Damping));
    CfgFile.write((char*)&Contacts, sizeof(Contacts));

    CfgFile.close();

    return true;

}

bool CServer::Create(Game::CTerrain *Terrain)
{

    dInitODE2(0);
    dAllocateODEDataForThread(dAllocateMaskAll);

    ODEWorld = dWorldCreate();

    Plane = dCreatePlane(Space, 0, 1, 0, 0);

//    dVector3 Position = {Terrain->GetDimensions() / 2.0f, 0.0f, Terrain->GetDimensions() / 2.0f};
//    dVector3 Extents = {Terrain->GetDimensions() / 2.0f, 16.0f, Terrain->GetDimensions() / 2.0f};

//    Space = dQuadTreeSpaceCreate(0, Position, Extents, 8);
    Space = dHashSpaceCreate(0);
    dHashSpaceSetLevels(Space, 2, 8);

//    EnvSpace = dQuadTreeSpaceCreate(Space, Position, Extents, 8);//dSimpleSpaceCreate(Space);
    EnvSpace = dHashSpaceCreate(Space);
    dHashSpaceSetLevels(EnvSpace, 2, 8);

    ContactGroup = dJointGroupCreate(0);

    SetTerrain(Terrain);
    SetPhysics();

    return true;

}

bool CServer::Destroy(void)
{

    ClientList.clear();

    if (ContactGroup) {
        dJointGroupDestroy(ContactGroup);
        ContactGroup = NULL;
    }

    if (Space) {
        dSpaceDestroy(Space);
        Space = NULL;
    }

//    if (EnvSpace) {
//        dSpaceDestroy(EnvSpace);
//        EnvSpace = NULL;
//    }


//    if (HeightField) {
//        dGeomDestroy(HeightField);
//        HeightField = NULL;
//    }

    if (HeightID) {
        dGeomHeightfieldDataDestroy(HeightID);
        HeightID = NULL;
    }

    if (Plane) {
        dGeomDestroy(Plane);
        Plane = NULL;
    }

    if (ODEWorld) {
        dWorldDestroy(ODEWorld);
        ODEWorld = NULL;
    }

    dCloseODE();

    return true;

}

bool CServer::SetTerrain(Game::CTerrain *Terrain)
{
    if (HeightField) {
        dGeomDestroy(HeightField);
        HeightField = NULL;
    }

    if (HeightID) {
        dGeomHeightfieldDataDestroy(HeightID);
        HeightID = NULL;
    }



    HeightID = dGeomHeightfieldDataCreate();
    HeightField = dCreateHeightfield(Space, HeightID, 1);
    dGeomHeightfieldDataSetBounds(HeightID, dReal(0.0f), dReal(256.0f));
    dGeomHeightfieldDataBuildByte(HeightID, &Terrain->GetHeightData(), 1, Terrain->GetDimensions() - 1.0f, Terrain->GetDimensions() - 1.0f, Terrain->GetDimensions(), Terrain->GetDimensions(), dReal(16.0f / 256.0f), 0.0f, 0.0f, 1);
    dGeomSetPosition(HeightField, dReal((Terrain->GetDimensions() - 1.0f) / 2.0f), dReal(0.0f), dReal((Terrain->GetDimensions() - 1.0f) / 2.0f));

    EnvData.Slip = World.Slip;
    dGeomSetData(HeightField, (void*)&EnvData);

    ContactGroup = dJointGroupCreate(0);

    return true;
}

void CServer::SetPhysics(void)
{
    dWorldSetGravity(ODEWorld, dReal(0), dReal(World.Gravity), dReal(0));
    dWorldSetERP(ODEWorld, dReal(World.ERP));
    dWorldSetCFM(ODEWorld, dReal(World.CFM));
    dWorldSetQuickStepNumIterations(ODEWorld, World.NumIterations);

    dWorldSetAutoDisableFlag(ODEWorld, Collisions.AutoDisableFlag);
    dWorldSetAutoDisableLinearThreshold(ODEWorld, dReal(Collisions.AutoDisableLinearThreshold));
    dWorldSetAutoDisableAngularThreshold(ODEWorld, dReal(Collisions.AutoDisableAngularThreshold));
    dWorldSetAutoDisableSteps(ODEWorld, Collisions.AutoDisableSteps);
    dWorldSetAutoDisableTime(ODEWorld, Collisions.AutoDisableTime);

    dWorldSetContactMaxCorrectingVel(ODEWorld, dReal(Contacts.MaxCorrectingVelocity));
    dWorldSetContactSurfaceLayer(ODEWorld, dReal(Contacts.SurfaceLayer));

//    dWorldSetDamping(ODEWorld, dReal(Damping.LinearDamping), dReal(Damping.AngularDamping));
    dWorldSetLinearDamping(ODEWorld, dReal(Damping.LinearDamping));
    dWorldSetLinearDampingThreshold(ODEWorld, dReal(Damping.LinearDampingThreshold));

    dWorldSetAngularDamping(ODEWorld, dReal(Damping.AngularDamping));
    dWorldSetAngularDampingThreshold(ODEWorld, dReal(Damping.AngularDampingThreshold));

//    dWorldSetQuickStepW(ODEWorld, 0.1f);
//    dWorldSetMaxAngularSpeed(ODEWorld, 1.0f);

}

bool CServer::RegisterClient(CClient *Client)
{
    bool Ret = Client->Initialize(ODEWorld, Space, EnvSpace);

    if (Ret) {
        ClientList.push_back(Client);
    }

    return Ret;
}

bool CServer::UnregisterClient(CClient *Client)
{
    ClientList.remove(Client);

    return true;
}

bool CServer::Update(dReal StepSize)
{
    dSpaceCollide(Space, this, &CollisionCallback);

    std::list <CClient*>::const_iterator iter = ClientList.begin();

    while (iter != ClientList.end()) {

        if (World.VariableStep) {
            (*iter)->Update(StepSize);
        } else {
            (*iter)->Update(World.StepSize);

        }

        iter++;

    }

    if (World.QuickStep) {
        if (World.VariableStep) {
            dWorldQuickStep(ODEWorld, StepSize);
        } else {
            dWorldQuickStep(ODEWorld, World.StepSize);
        }
    } else {
        if (World.VariableStep) {
            dWorldStep(ODEWorld, StepSize);
        } else {
            dWorldStep(ODEWorld, World.StepSize);
        }
    }

    dJointGroupEmpty(ContactGroup);

    return true;

}

void CServer::CheckCollisions(void *Data, dGeomID o1, dGeomID o2)
{

    if (dGeomIsSpace(o1) || dGeomIsSpace(o2)) {

        dSpaceCollide2(o1, o2, Data, &CollisionCallback);

    } else {

        int Mode = 0;
        dReal Slip1 = 0;
        dReal Slip2 = 0;

        dBodyID Body1 = dGeomGetBody(o1);
        dBodyID Body2 = dGeomGetBody(o2);

        GeomData* Data1 = (GeomData*)dGeomGetData(o1);
        GeomData* Data2 = (GeomData*)dGeomGetData(o2);

        if (Data1 != NULL) {
            Slip1 = Data1->Slip;
        }

        if (Data2 != NULL) {
            Slip2 = Data2->Slip;
        }

        if (Slip1 != 0) {
            Mode |= dContactSlip1;
        }

        if (Slip2 != 0) {
            Mode |= dContactSlip2;
        }

        dContact Contact[Contacts.NumContacts];

        int N = dCollide(o1, o2, Contacts.NumContacts, &Contact[0].geom, sizeof(dContact));

        if (N > 0) {
            for (int i = 0; i < N; i++) {

                Contact[i].surface.mode = Mode | dContactBounce | dContactSoftERP | dContactSoftCFM | dContactApprox1;

                Contact[i].surface.mu = dInfinity;
                Contact[i].surface.mu2 = dInfinity;
                Contact[i].surface.slip1 = Slip1;
                Contact[i].surface.slip2 = Slip2;
                Contact[i].surface.bounce_vel = Collisions.BounceVelocity;
                Contact[i].surface.bounce = Collisions.Bounce;
                Contact[i].surface.soft_cfm = Collisions.SoftCFM;
                Contact[i].surface.soft_erp = Collisions.SoftERP;

                dJointID C = dJointCreateContact(ODEWorld, ContactGroup, &Contact[i]);
                dJointAttach(C, Body1, Body2);

            }
        }
    }
}

void CServer::CollisionCallback(void *Data, dGeomID o1, dGeomID o2)
{

    CServer* ServerPtr = (CServer*)Data;
    ServerPtr->CheckCollisions(ServerPtr, o1, o2);

}


