
#ifndef VEHICLE_HPP
#define VEHICLE_HPP

#define dDOUBLE

#include <Ode/Ode.h>
#include <gtc/constants.hpp>

#include "../Engine/Input.hpp"
#include "../Game/Models.hpp"
#include "../Game/Trucks.hpp"

#include "Server.hpp"
#include "Client.hpp"

extern GLFWwindow *GLWindow;
extern Engine::CInput *Input;

namespace Physics
{

class CVehicle : public CClient, Game::CTruck
{

public:

    struct {
        bool Automatic;
        int Gear;
        int GearCount;
        double GearAccel[3][3];
        double GearNext[3][3];
        double Speed;
        double Rpms;
        double MaxRpms;
        double Brake;
        double MaxSpeed;
        double ReverseSpeed;
        double Drag;
        double HorsePower;
        double Acceleration;
    } Transmission;

    struct Wheel {
        glm::vec3 Position;
        double Mass;
        double SteerMultiplier;
        double DriveMultiplier;
        double Damping;
        double Error;
        double StopERP;
        double StopCFM;
        double Bounce;
        double CFM;
        double FMax;
        double FudgeFactor;
        dGeomID Geom;
        dBodyID Body;
        dGeomID ShockUpper[2];
        dGeomID ShockLower[2];
        GeomData Data;
    };

    struct Axle {
        glm::vec3 Position;
        double Mass;
        double MinSteer;
        double MaxSteer;
        double SteerSpeed;
        double CenterSpeed;
        double SteerAngle;
        double CurrentSteer;
        dGeomID Geom;
        dBodyID Body;
        GeomData Data;
        Wheel Wheels[2];
        dJointID Motor[2];
        dJointID Hub[2];
        dJointID FourLink[2];
        dJointID DriveShaft;
    };

    struct {
        double MassScale;
        double Mass;
        double COGMass;
        double Speed;
        double WheelForce;
        dBodyID Body;
        dBodyID COG;
        dJointID COGJoint;
        GeomData Data;
        dGeomID Geom;
        Axle Axles[2];
    } Vehicle;


    dSpaceID VehicleSpace;
    dGeomID CollisionPoints[12];

    float Velocity;
    int Boundaries;
    glm::vec3 Center, Rotation;

    double DownShift(unsigned int Index);
    double UpShift(unsigned int Index);

    double BestGear(unsigned int Index);
    void CheckGear(double Delta);
    void AutomaticGear(void);

    void AddGeom(dGeomID Geom, dBodyID Body, dReal xOffset, dReal yOffset, dReal zOffset, dMatrix3 R);

    void SetOrientation(glm::vec3 Position, glm::vec3 Rotation);
    void SetPosition(glm::vec3 Centrum);

    void Cylinder(glm::vec3 Start, glm::vec3 End, float Radius, int Subdivisions, glm::vec4 Color, bool Lighted);

public:

    CVehicle(void);
    virtual ~CVehicle(void);

    bool LoadPhysics(std::string Filename);
    bool SavePhysics(std::string Filename);

    bool LoadTruck(Game::CPodPool &PodPool, std::string Filename)
    {
        return Load(PodPool, Filename);
    }

    bool Create(glm::vec3 Center, glm::vec3 Rotate, int Boundaries);
    void Destroy(void);
    bool Defaults(std::string Filename = "Default.vehicle");

    void SetPhysics(void);

    float GetVelocity(void);

    int GetGear(void)
    {
        return Transmission.Gear;
    }

    float GetSpeed(void)
    {
        return Transmission.Speed;
    }

    float GetRpms(void)
    {
        return Transmission.Rpms;
    }

    void Reset(void);
    void Flip(void);

    virtual bool Update(dReal Delta);
    void Render(void);

    const glm::vec3 GetPosition(void)
    {

        const dReal *Position = dBodyGetPosition(Vehicle.Body);
        return glm::vec3(Position[0], Position[1], Position[2]);
    }

    const glm::vec3 GetWheelPosition(unsigned int Axle, unsigned int Wheel)
    {
        const dReal *Position = dBodyGetPosition(Vehicle.Axles[Axle].Wheels[Wheel].Body);
        return glm::vec3(Position[0], Position[1], Position[2]);
    }



};
}

#endif
