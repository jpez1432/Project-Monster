

#include "Vehicle.hpp"

using namespace Physics;

double CVehicle::DownShift(unsigned int Index)
{
    return -Transmission.GearAccel[Index][1] / (Transmission.GearNext[Index][2] - Transmission.GearNext[Index][1]);
}

double CVehicle::UpShift(unsigned int Index)
{
    return (Transmission.GearAccel[Index][1] - Transmission.GearAccel[Index][0]) / Transmission.GearNext[Index][1];
}

double CVehicle::BestGear(unsigned int Index)
{
    if ((Index < (unsigned int)Transmission.GearCount) && (Index > 0)) {
        return (Transmission.GearAccel[Index + 1][0] - Transmission.GearAccel[Index][1] + DownShift(Index) * Transmission.GearNext[Index][1]) / (DownShift(Index) - UpShift(Index + 1));
    }

    if (Index == (unsigned int)Transmission.GearCount) {
        return Transmission.GearNext[Transmission.GearCount][2];
    }

    return 0;
}

void CVehicle::CheckGear(double Delta)
{
    unsigned int Index = 0;

    for (unsigned int i = 0; i < 3; i++) {
        if (Transmission.Speed > Transmission.GearNext[Transmission.Gear][i]) {
            Index = i;
        }
    }

    if (Index == 2) {
        Transmission.Speed += Delta * -Transmission.Drag * 3600 / 1000;
    } else {
        double Value = (Transmission.GearNext[Transmission.Gear][Index + 1] - Transmission.GearNext[Transmission.Gear][Index]) / (Transmission.GearAccel[Transmission.Gear][Index + 1] - Transmission.GearAccel[Transmission.Gear][Index]);
        Transmission.Acceleration = (Transmission.Speed - Transmission.GearNext[Transmission.Gear][Index]) / Value + Transmission.GearAccel[Transmission.Gear][Index];
    }

//    if (Transmission.Speed > Transmission.MaxSpeed) {
//        Transmission.Speed = Transmission.MaxSpeed;
//    }

    Transmission.Rpms = Transmission.Speed * Transmission.MaxRpms / Transmission.GearNext[Transmission.Gear][2];

//    if (Transmission.Rpms > Transmission.MaxRpms) {
//        Transmission.Rpms = Transmission.MaxRpms;
//    }

//    std::cout << Transmission.Speed << ", " << Transmission.Rpms << std::endl;

}

void CVehicle::AutomaticGear(void)
{
    if (!Transmission.Automatic) {
        return;
    }

    if (Transmission.Speed >= BestGear(Transmission.Gear) && (Transmission.Gear < Transmission.GearCount)) {
        Transmission.Gear++;
    }

    if ((Transmission.Gear > 1) && (Transmission.Speed < BestGear(Transmission.Gear - 1))) {
        Transmission.Gear--;
    }
}

CVehicle::CVehicle(void) : Velocity(0.0f)
{
    Vehicle.Body = NULL;

    for (int i = 0; i < 12; i++) {
        CollisionPoints[i] = NULL;
    }

    for (unsigned int i = 0; i < 2; i++) {

        Vehicle.Axles[i].Body = NULL;
        Vehicle.Axles[i].Geom = NULL;
        Vehicle.Axles[i].DriveShaft = NULL;

        for (unsigned int j = 0; j < 2; j++) {

            for (unsigned int k = 0; k < 2; k++) {

                Vehicle.Axles[i].Wheels[j].ShockLower[k] = NULL;
                Vehicle.Axles[i].Wheels[j].ShockUpper[k] = NULL;

            }

            Vehicle.Axles[i].Wheels[j].Body = NULL;
            Vehicle.Axles[i].Wheels[j].Geom = NULL;
            Vehicle.Axles[i].Motor[j] = NULL;
            Vehicle.Axles[i].Hub[j] = NULL;
            Vehicle.Axles[i].FourLink[j] = NULL;

        }
    }

    VehicleSpace = NULL;

}

CVehicle::~CVehicle(void)
{
    Destroy();
}

void CVehicle::Destroy(void)
{

    if (Vehicle.Body) {
        dBodyDestroy(Vehicle.Body);
        Vehicle.Body = NULL;
    }

    for (int i = 0; i < 12; i++) {
        dGeomDestroy(CollisionPoints[i]);
        CollisionPoints[i] = NULL;
    }

    for (unsigned int i = 0; i < 2; i++) {

        if (Vehicle.Axles[i].Body) {
            dBodyDestroy(Vehicle.Axles[i].Body);
            Vehicle.Axles[i].Body = NULL;
        }

        if (Vehicle.Axles[i].Geom) {
            dGeomDestroy(Vehicle.Axles[i].Geom);
            Vehicle.Axles[i].Geom = NULL;
        }

        if (Vehicle.Axles[i].DriveShaft) {
            dJointDestroy(Vehicle.Axles[i].DriveShaft);
            Vehicle.Axles[i].DriveShaft = NULL;
        }

        for (unsigned int j = 0; j < 2; j++) {

            for (unsigned int k = 0; k < 2; k++) {

                if (Vehicle.Axles[i].Wheels[j].ShockLower[k]) {
                    dGeomDestroy(Vehicle.Axles[i].Wheels[j].ShockLower[k]);
                    Vehicle.Axles[i].Wheels[j].ShockLower[k] = NULL;
                }

                if (Vehicle.Axles[i].Wheels[j].ShockUpper[k]) {
                    dGeomDestroy(Vehicle.Axles[i].Wheels[j].ShockUpper[k]);
                    Vehicle.Axles[i].Wheels[j].ShockUpper[k] = NULL;
                }
            }

            if (Vehicle.Axles[i].Wheels[j].Body) {
                dBodyDestroy(Vehicle.Axles[i].Wheels[j].Body);
                Vehicle.Axles[i].Wheels[j].Body = NULL;
            }

            if (Vehicle.Axles[i].Wheels[j].Geom) {
                dGeomDestroy(Vehicle.Axles[i].Wheels[j].Geom);
                Vehicle.Axles[i].Wheels[j].Geom = NULL;
            }

            if (Vehicle.Axles[i].Motor[j]) {
                dJointDestroy(Vehicle.Axles[i].Motor[j]);
                Vehicle.Axles[i].Motor[j] = NULL;
            }

            if (Vehicle.Axles[i].Hub[j]) {
                dJointDestroy(Vehicle.Axles[i].Hub[j]);
                Vehicle.Axles[i].Hub[j] = NULL;
            }

            if (Vehicle.Axles[i].FourLink[j]) {
                dJointDestroy(Vehicle.Axles[i].FourLink[j]);
                Vehicle.Axles[i].FourLink[j] = NULL;
            }

        }
    }

    if (VehicleSpace) {
        dSpaceDestroy(VehicleSpace);
        VehicleSpace = NULL;
    }
}

bool CVehicle::Defaults(std::string Filename)
{

//    ZeroMemory(&Vehicle, sizeof(Vehicle));

    Vehicle.MassScale = 1.0f;
    Vehicle.Mass = 7500;
    Vehicle.Data.Slip = 0.000001f;

    Vehicle.Axles[0].Mass = 600;
    Vehicle.Axles[0].Data.Slip = 0.000001f;
    Vehicle.Axles[0].MinSteer = -0.4f;
    Vehicle.Axles[0].MaxSteer = 0.4f;
    Vehicle.Axles[0].SteerSpeed = 0.25f;
    Vehicle.Axles[0].CenterSpeed = 0.55f;

    Vehicle.Axles[0].Wheels[0].DriveMultiplier = 1;
    Vehicle.Axles[0].Wheels[0].SteerMultiplier = 1;
    Vehicle.Axles[0].Wheels[0].Data.Slip = 0.000001f;
    Vehicle.Axles[0].Wheels[0].Damping = 0.000068f;
    Vehicle.Axles[0].Wheels[0].Error = 0.9f;
    Vehicle.Axles[0].Wheels[0].StopERP = 0.9f;
    Vehicle.Axles[0].Wheels[0].StopCFM = 0.00001f;
    Vehicle.Axles[0].Wheels[0].Bounce = 0.25f;
    Vehicle.Axles[0].Wheels[0].CFM = 0.00001f;
    Vehicle.Axles[0].Wheels[0].FMax = 100.0f;
    Vehicle.Axles[0].Wheels[0].FudgeFactor = 0.8f;
    Vehicle.Axles[0].Wheels[0].Mass = 750;

    Vehicle.Axles[0].Wheels[1].DriveMultiplier = 1;
    Vehicle.Axles[0].Wheels[1].SteerMultiplier = 1;
    Vehicle.Axles[0].Wheels[1].Data.Slip = 0.000001f;
    Vehicle.Axles[0].Wheels[1].Damping = 0.000068f;
    Vehicle.Axles[0].Wheels[1].Error = 0.9f;
    Vehicle.Axles[0].Wheels[1].StopERP = 0.9f;
    Vehicle.Axles[0].Wheels[1].StopCFM = 0.00001f;
    Vehicle.Axles[0].Wheels[1].Bounce = 0.25f;
    Vehicle.Axles[0].Wheels[1].CFM = 0.00001f;
    Vehicle.Axles[0].Wheels[1].FMax = 100.0f;
    Vehicle.Axles[0].Wheels[1].FudgeFactor = 0.8f;
    Vehicle.Axles[0].Wheels[1].Mass = 750;

    Vehicle.Axles[1].Mass = 600;
    Vehicle.Axles[1].Data.Slip = 0.000001f;
    Vehicle.Axles[1].MinSteer = -0.4f;
    Vehicle.Axles[1].MaxSteer = 0.4f;
    Vehicle.Axles[1].SteerSpeed = 0.25f;
    Vehicle.Axles[1].CenterSpeed = 0.55f;

    Vehicle.Axles[1].Wheels[0].DriveMultiplier = 1;
    Vehicle.Axles[1].Wheels[0].SteerMultiplier = -1;
    Vehicle.Axles[1].Wheels[0].Data.Slip = 0.000001f;
    Vehicle.Axles[1].Wheels[0].Damping = 0.000068f;
    Vehicle.Axles[1].Wheels[0].Error = 0.9f;
    Vehicle.Axles[1].Wheels[0].StopERP = 0.9f;
    Vehicle.Axles[1].Wheels[0].StopCFM = 0.00001f;
    Vehicle.Axles[1].Wheels[0].Bounce = 0.25f;
    Vehicle.Axles[1].Wheels[0].CFM = 0.00001f;
    Vehicle.Axles[1].Wheels[0].FMax = 100.0f;
    Vehicle.Axles[1].Wheels[0].FudgeFactor = 0.8f;
    Vehicle.Axles[1].Wheels[0].Mass = 750;

    Vehicle.Axles[1].Wheels[1].DriveMultiplier = 1;
    Vehicle.Axles[1].Wheels[1].SteerMultiplier = -1;
    Vehicle.Axles[1].Wheels[1].Data.Slip = 0.000001f;
    Vehicle.Axles[1].Wheels[1].Damping = 0.000068f;
    Vehicle.Axles[1].Wheels[1].Error = 0.9f;
    Vehicle.Axles[1].Wheels[1].StopERP = 0.9f;
    Vehicle.Axles[1].Wheels[1].StopCFM = 0.00001f;
    Vehicle.Axles[1].Wheels[1].Bounce = 0.25f;
    Vehicle.Axles[1].Wheels[1].CFM = 0.00001f;
    Vehicle.Axles[1].Wheels[1].FMax = 100.0f;
    Vehicle.Axles[1].Wheels[1].FudgeFactor = 0.8f;
    Vehicle.Axles[1].Wheels[1].Mass = 750;

    ZeroMemory(&Transmission, sizeof(Transmission));

    Transmission.Automatic = true;
    Transmission.Gear = 1;
    Transmission.GearCount = 2;
    Transmission.Speed = 0.0f;
    Transmission.Rpms = 0.0f;
    Transmission.MaxRpms = 9500.0f;
    Transmission.Brake = -100;
    Transmission.MaxSpeed = 70.0f;
    Transmission.ReverseSpeed = -25.0f;
    Transmission.Drag = -10.0f;
    Transmission.HorsePower = 1750.0f;
    Transmission.Acceleration = 1.0f;

    float Scale = Transmission.HorsePower;
    Transmission.GearAccel[0][0] = 1 * Scale;
    Transmission.GearAccel[1][0] = 2.059000 * Transmission.GearAccel[0][0];
    Transmission.GearAccel[2][0] = 1.625000 * Transmission.GearAccel[0][0];;

    Transmission.GearAccel[0][1] = 1.0 * Scale;
    Transmission.GearAccel[1][1] = 1.100000 * Transmission.GearAccel[0][1];
    Transmission.GearAccel[2][1] = 0.900000 * Transmission.GearAccel[0][1];

    Transmission.GearAccel[0][2] = 0.0f;
    Transmission.GearAccel[1][2] = 0.0f;
    Transmission.GearAccel[2][2] = 0.0f;

    Transmission.GearNext[0][0] = 0.0f;
    Transmission.GearNext[1][0] = 0.0f;
    Transmission.GearNext[2][0] = 0.0f;

    Scale = 1.0f;
    Transmission.GearNext[0][1] = 0.0f * Scale;
    Transmission.GearNext[1][1] = 15.0f * Scale;
    Transmission.GearNext[2][1] = 35.0f * Scale;

//    Scale = 2.0f;
    Transmission.GearNext[0][2] = 10.0f * Scale;
    Transmission.GearNext[1][2] = 65.0f * Scale;
    Transmission.GearNext[2][2] = Transmission.MaxSpeed * Scale;

//    SavePhysics("Default.vehicle");


    return true;

}

void CVehicle::SetPhysics(void)
{
    dMass M;
    glm::vec3 Min, Max, Dimensions;

    Dimensions = GetBody().GetBounds(Min, Max);// * 32.0f;

    dMassSetBoxTotal(&M, Vehicle.Mass * Vehicle.MassScale, Dimensions.x, Dimensions.y, Dimensions.z);
    dBodySetMass(Vehicle.Body, &M);

//    dGeomSetData(Vehicle.Geom, (void*)&Vehicle.Data);
    for (int i = 0; i < 12; i++) {
        dGeomSetData(CollisionPoints[i], (void*)&Vehicle.Data);
    }

    for (unsigned int i = 0; i < 2; i++) {

        Dimensions = GetAxle().GetBounds(Min, Max);// * 32.0f;

        dGeomSetData(Vehicle.Axles[i].Geom, (void*)&Vehicle.Axles[i].Data);

        dMassSetCylinderTotal(&M, Vehicle.Axles[i].Mass * Vehicle.MassScale, 1, Dimensions.y / 2.0f, Dimensions.x / 2.0f);
        dBodySetMass(Vehicle.Axles[i].Body, &M);

        for (unsigned int j = 0; j < 2; j++) {

            Dimensions = GetWheel(j).GetBounds(Min, Max);// * 32.0f;

            dGeomSetData(Vehicle.Axles[i].Wheels[j].Geom, (void*)&Vehicle.Axles[i].Wheels[j].Data);

            if (j == 0) {
                dMassSetCylinderTotal(&M, Vehicle.Axles[i].Wheels[j].Mass * Vehicle.MassScale, -1, Dimensions.y / 2.0f, Dimensions.x / 2.0f);
            } else {
                dMassSetCylinderTotal(&M, Vehicle.Axles[i].Wheels[j].Mass * Vehicle.MassScale, 1, Dimensions.y / 2.0f, Dimensions.x / 2.0f);
            }
            dBodySetMass(Vehicle.Axles[i].Wheels[j].Body, &M);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamLoStop, 0);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamHiStop, 0);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionERP, Vehicle.Axles[i].Wheels[j].Error);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionCFM, Vehicle.Axles[i].Wheels[j].Damping);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopERP, Vehicle.Axles[i].Wheels[j].Error);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopCFM, Vehicle.Axles[i].Wheels[j].StopCFM);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamBounce, Vehicle.Axles[i].Wheels[j].Bounce);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamCFM, Vehicle.Axles[i].Wheels[j].CFM);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFMax, Vehicle.Axles[i].Wheels[j].FMax);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFudgeFactor, Vehicle.Axles[i].Wheels[j].FudgeFactor);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamLoStop2, 0);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamHiStop2, 0);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionERP2, Vehicle.Axles[i].Wheels[j].Error);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionCFM2, Vehicle.Axles[i].Wheels[j].Damping);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopERP2, Vehicle.Axles[i].Wheels[j].Error);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopCFM2, Vehicle.Axles[i].Wheels[j].StopCFM);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamBounce2, Vehicle.Axles[i].Wheels[j].Bounce);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamCFM2, Vehicle.Axles[i].Wheels[j].CFM);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFMax2, Vehicle.Axles[i].Wheels[j].FMax);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFudgeFactor2, Vehicle.Axles[i].Wheels[j].FudgeFactor);
        }
    }

}

bool CVehicle::LoadPhysics(std::string Filename)
{
    std::string PhysicsFile = RootDir + ResourceDir + "Physics\\" + Filename;

    if(Config->Physics.Defaults)
    {
        Defaults(Filename);
        return true;
    }

    std::ifstream CfgFile(PhysicsFile.c_str(), std::ios::in | std::ios::binary);

    if (CfgFile.fail()) {
        Logger->Error("Error Loading Vehicle Physics, Loading Defaults...", true, true, false);
        Defaults(Filename);
        return true;
    }

    CfgFile.read((char*)&Vehicle.MassScale, sizeof(Vehicle.MassScale));
    CfgFile.read((char*)&Vehicle.Mass, sizeof(Vehicle.Mass));
    CfgFile.read((char*)&Vehicle.Data.Slip, sizeof(Vehicle.Data.Slip));

    for (unsigned int i = 0; i < 2; i++) {

        CfgFile.read((char*)&Vehicle.Axles[i].Mass, sizeof(Vehicle.Axles[i].Mass));
        CfgFile.read((char*)&Vehicle.Axles[i].MinSteer, sizeof(Vehicle.Axles[i].MinSteer));
        CfgFile.read((char*)&Vehicle.Axles[i].MaxSteer, sizeof(Vehicle.Axles[i].MaxSteer));
        CfgFile.read((char*)&Vehicle.Axles[i].SteerSpeed, sizeof(Vehicle.Axles[i].SteerSpeed));
        CfgFile.read((char*)&Vehicle.Axles[i].CenterSpeed, sizeof(Vehicle.Axles[i].CenterSpeed));
        CfgFile.read((char*)&Vehicle.Axles[i].Data.Slip, sizeof(Vehicle.Axles[i].Data.Slip));

        for (unsigned int j = 0; j < 2; j++) {

            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Mass, sizeof(Vehicle.Axles[i].Wheels[j].Mass));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].SteerMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].SteerMultiplier));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].DriveMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].DriveMultiplier));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Damping, sizeof(Vehicle.Axles[i].Wheels[j].Damping));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Error, sizeof(Vehicle.Axles[i].Wheels[j].Error));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].StopERP, sizeof(Vehicle.Axles[i].Wheels[j].StopERP));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].StopCFM, sizeof(Vehicle.Axles[i].Wheels[j].StopCFM));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Bounce, sizeof(Vehicle.Axles[i].Wheels[j].Bounce));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].CFM, sizeof(Vehicle.Axles[i].Wheels[j].CFM));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].FMax, sizeof(Vehicle.Axles[i].Wheels[j].FMax));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].FudgeFactor, sizeof(Vehicle.Axles[i].Wheels[j].FudgeFactor));
            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Data.Slip, sizeof(Vehicle.Axles[i].Wheels[j].Data.Slip));
        }
    }

    CfgFile.read((char*)&Transmission, sizeof(Transmission));

    CfgFile.close();

    return true;
}

bool CVehicle::SavePhysics(std::string Filename)
{
    std::string PhysicsFile = RootDir + ResourceDir + "Physics\\" + Filename;

    std::ofstream CfgFile(PhysicsFile.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if (CfgFile.fail()) {
        Logger->Error("Error Saving Vehicle Physics, Aborting...", true, true, false);
        return false;
    }

    CfgFile.write((char*)&Vehicle.MassScale, sizeof(Vehicle.MassScale));
    CfgFile.write((char*)&Vehicle.Mass, sizeof(Vehicle.Mass));
    CfgFile.write((char*)&Vehicle.Data.Slip, sizeof(Vehicle.Data.Slip));

    for (unsigned int i = 0; i < 2; i++) {
        CfgFile.write((char*)&Vehicle.Axles[i].Mass, sizeof(Vehicle.Axles[i].Mass));
        CfgFile.write((char*)&Vehicle.Axles[i].MinSteer, sizeof(Vehicle.Axles[i].MinSteer));
        CfgFile.write((char*)&Vehicle.Axles[i].MaxSteer, sizeof(Vehicle.Axles[i].MaxSteer));
        CfgFile.write((char*)&Vehicle.Axles[i].SteerSpeed, sizeof(Vehicle.Axles[i].SteerSpeed));
        CfgFile.write((char*)&Vehicle.Axles[i].CenterSpeed, sizeof(Vehicle.Axles[i].CenterSpeed));
        CfgFile.write((char*)&Vehicle.Axles[i].Data.Slip, sizeof(Vehicle.Axles[i].Data.Slip));

        for (unsigned int j = 0; j < 2; j++) {
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Mass, sizeof(Vehicle.Axles[i].Wheels[j].Mass));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].SteerMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].SteerMultiplier));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].DriveMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].DriveMultiplier));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Damping, sizeof(Vehicle.Axles[i].Wheels[j].Damping));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Error, sizeof(Vehicle.Axles[i].Wheels[j].Error));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].StopERP, sizeof(Vehicle.Axles[i].Wheels[j].StopERP));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].StopCFM, sizeof(Vehicle.Axles[i].Wheels[j].StopCFM));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Bounce, sizeof(Vehicle.Axles[i].Wheels[j].Bounce));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].CFM, sizeof(Vehicle.Axles[i].Wheels[j].CFM));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].FMax, sizeof(Vehicle.Axles[i].Wheels[j].FMax));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].FudgeFactor, sizeof(Vehicle.Axles[i].Wheels[j].FudgeFactor));
            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Data.Slip, sizeof(Vehicle.Axles[i].Wheels[j].Data.Slip));
        }
    }

    CfgFile.write((char*)&Transmission, sizeof(Transmission));

    CfgFile.close();

    return true;
}


bool CVehicle::Create(glm::vec3 Centrum, glm::vec3 Rotation, int Boundaries)
{

    Centrum.y += 0.25f;
    this->Center = Centrum;
    this->Rotation = Rotation;
    this->Boundaries = Boundaries;

    dMass M;
    dMatrix3 R1;
    glm::vec3 Min, Max, Dimensions;

    Vehicle.Axles[1].Wheels[0].Position = GetRearAxleLeftTire();
    Vehicle.Axles[1].Wheels[1].Position = GetRearAxleRightTire();
    Vehicle.Axles[0].Wheels[0].Position = GetFrontAxleLeftTire();
    Vehicle.Axles[0].Wheels[1].Position = GetFrontAxleRightTire();

    Vehicle.Axles[0].Position = glm::vec3(0.0f, GetFrontAxleRightTire().y, GetFrontAxleRightTire().z);
    Vehicle.Axles[1].Position = glm::vec3(0.0f, GetRearAxleRightTire().y, GetRearAxleRightTire().z);

    VehicleSpace = dSimpleSpaceCreate(Space);

    Vehicle.Body = dBodyCreate(World);

//    dVector3 *Vertices = new dVector3[GetBody().Header.NumVertices];
//    dTriIndex *Indices = new dTriIndex[GetBody().Faces.size() * 3];
//
//    for(int i = 0; i != GetBody().Header.NumVertices; i++) {
//        Vertices[i][0] = dReal(GetBody().Vertices[i].x);
//        Vertices[i][1] = dReal(GetBody().Vertices[i].y);
//        Vertices[i][2] = dReal(GetBody().Vertices[i].z);
//    }
//
//    int v = 0;
//
//    for(unsigned int i = 0; i != GetBody().Faces.size(); i++) {
//        Indices[v++] = GetBody().Faces[i].Vertices[2].Index;
//        Indices[v++] = GetBody().Faces[i].Vertices[1].Index;
//        Indices[v++] = GetBody().Faces[i].Vertices[0].Index;
//    }
//
//    dTriMeshDataID Data = dGeomTriMeshDataCreate();
//    dGeomTriMeshDataBuildSimple(Data, *Vertices, GetBody().Header.NumVertices, Indices, GetBody().Faces.size() * 3);
//    Vehicle.Geom = dCreateTriMesh(0, Data, 0, 0, 0);
//
//    dGeomSetBody(Vehicle.Geom, Vehicle.Body);
//    dSpaceAdd(VehicleSpace, Vehicle.Geom);

    Dimensions = GetBody().GetBounds(Min, Max);
    dMassSetBoxTotal(&M, Vehicle.Mass * Vehicle.MassScale, Dimensions.x, Dimensions.y, Dimensions.z);
    dBodySetMass(Vehicle.Body, &M);

    for (int i = 0; i < 12; i++) {
        CollisionPoints[i] = dCreateSphere(VehicleSpace, 0.00001f);
        dGeomSetBody(CollisionPoints[i], Vehicle.Body);
        dGeomSetOffsetPosition(CollisionPoints[i], GetScrapePoint(i).x,  GetScrapePoint(i).y,  GetScrapePoint(i).z);
    }

    for (unsigned int i = 0; i < 2; i++) {

        Dimensions = GetAxle().GetBounds(Min, Max);

        Vehicle.Axles[i].Body = dBodyCreate(World);
//        dBodySetFiniteRotationMode(Vehicle.Axles[i].Body, 1);

//        dVector3 *Vertices = new dVector3[GetAxle().Header.NumVertices];
//        dTriIndex *Indices = new dTriIndex[GetAxle().Faces.size() * 3];
//
//        for(int i = 0; i != GetAxle().Header.NumVertices; i++) {
//            Vertices[i][0] = dReal(GetAxle().Vertices[i].x);
//            Vertices[i][1] = dReal(GetAxle().Vertices[i].y);
//            Vertices[i][2] = dReal(GetAxle().Vertices[i].z);
//        }
//
//        int v = 0;
//
//        for(unsigned int i = 0; i != GetAxle().Faces.size(); i++) {
//            Indices[v++] = GetAxle().Faces[i].Vertices[2].Index;
//            Indices[v++] = GetAxle().Faces[i].Vertices[1].Index;
//            Indices[v++] = GetAxle().Faces[i].Vertices[0].Index;
//        }
//
//        dTriMeshDataID Data = dGeomTriMeshDataCreate();
//        dGeomTriMeshDataBuildSimple(Data, *Vertices, GetAxle().Header.NumVertices, Indices, GetAxle().Faces.size() * 3);
//        Vehicle.Axles[i].Geom = dCreateTriMesh(0, Data, 0, 0, 0);

        Vehicle.Axles[i].Geom = dCreateCylinder(VehicleSpace, Dimensions.y / 2.0f, Dimensions.x);
        dGeomSetBody(Vehicle.Axles[i].Geom, Vehicle.Axles[i].Body);

        dMassSetCylinderTotal(&M, Vehicle.Axles[i].Mass * Vehicle.MassScale, 1, Dimensions.y / 2.0f, Dimensions.x);
        dBodySetMass(Vehicle.Axles[i].Body, &M);

        dRSetIdentity(R1);
        dRFromEulerAngles(R1, 0.0f, glm::radians(180.0f) * i, 0.0f);
        dGeomSetRotation(Vehicle.Axles[i].Geom, R1);

        for (unsigned int j = 0; j < 2; j++) {

            Dimensions = GetWheel(j).GetBounds(Min, Max);

            Vehicle.Axles[i].Wheels[j].Body = dBodyCreate(World);
            dBodySetFiniteRotationMode(Vehicle.Axles[i].Wheels[j].Body, 1);

//            dVector3 *Vertices = new dVector3[GetWheel(j).Header.NumVertices];
//            dTriIndex *Indices = new dTriIndex[GetWheel(j).Faces.size() * 3];
//
//            for(int i = 0; i != GetWheel(j).Header.NumVertices; i++) {
//                Vertices[i][0] = dReal(GetWheel(j).Vertices[i].x);
//                Vertices[i][1] = dReal(GetWheel(j).Vertices[i].y);
//                Vertices[i][2] = dReal(GetWheel(j).Vertices[i].z);
//            }
//
//            int v = 0;
//
//            for(unsigned int i = 0; i != GetWheel(j).Faces.size(); i++) {
//                Indices[v++] = GetWheel(j).Faces[i].Vertices[2].Index;
//                Indices[v++] = GetWheel(j).Faces[i].Vertices[1].Index;
//                Indices[v++] = GetWheel(j).Faces[i].Vertices[0].Index;
//            }
//
//            dTriMeshDataID Data = dGeomTriMeshDataCreate();
//            dGeomTriMeshDataBuildSimple(Data, *Vertices, GetWheel(j).Header.NumVertices, Indices, GetWheel(j).Faces.size() * 3);
//            Vehicle.Axles[i].Wheels[j].Geom = dCreateTriMesh(0, Data, 0, 0, 0);


            Vehicle.Axles[i].Wheels[j].Geom = dCreateCylinder(VehicleSpace, Dimensions.y / 2.0f, Dimensions.x);
            dGeomSetBody(Vehicle.Axles[i].Wheels[j].Geom, Vehicle.Axles[i].Wheels[j].Body);

            dMassSetCylinderTotal(&M, Vehicle.Axles[i].Wheels[j].Mass * Vehicle.MassScale, 1, Dimensions.y / 2.0f, Dimensions.x);
            dBodySetMass(Vehicle.Axles[i].Wheels[j].Body, &M);

            dMatrix3 R1;
            dRSetIdentity(R1);
            dRFromEulerAngles(R1, 0.0f, glm::radians(90.0f), 0.0f);
//            dGeomSetRotation(Vehicle.Axles[i].Wheels[j].Geom, R1);
            dBodySetRotation(Vehicle.Axles[i].Wheels[j].Body, R1);
        }
    }

//    SetPosition(glm::vec3(0));
    SetOrientation(glm::vec3(0), glm::vec3(0));

    for (unsigned int i = 0; i < 2; i++) {

        Vehicle.Axles[i].DriveShaft = dJointCreateBall(World, 0);
        dJointAttach(Vehicle.Axles[i].DriveShaft, Vehicle.Body, Vehicle.Axles[i].Body);
        dJointSetBallAnchor(Vehicle.Axles[i].DriveShaft, GetDriveShaftPos().x, GetDriveShaftPos().y, GetDriveShaftPos().z);

//        Vehicle.Axles[i].DriveShaft = dJointCreateHinge(World, 0);
//        dJointAttach(Vehicle.Axles[i].DriveShaft, Vehicle.Body, Vehicle.Axles[i].Body);
//        dJointSetHingeAnchor(Vehicle.Axles[i].DriveShaft, GetDriveShaftPos().x, GetDriveShaftPos().y, GetDriveShaftPos().z);
//        dJointSetHingeAxis(Vehicle.Axles[i].DriveShaft, 0, 0, 1);

        for (unsigned int j = 0; j < 2; j++) {

            Vehicle.Axles[i].Hub[j] = dJointCreateBall(World, 0);
            dJointAttach(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Body, Vehicle.Axles[i].Wheels[j].Body);
            dJointSetBallAnchor(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z);

//            Vehicle.Axles[i].Hub[j] = dJointCreateHinge(World, 0);
//            dJointAttach(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Body, Vehicle.Axles[i].Wheels[j].Body);
//            dJointSetHingeAnchor(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z );
//            dJointSetHingeAxis(Vehicle.Axles[i].Hub[j], 1, 0, 0);

//            Vehicle.Axles[i].Hub[j] = dJointCreateUniversal(World, 0);
//            dJointAttach(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Body, Vehicle.Axles[i].Wheels[j].Body);
//            dJointSetUniversalAnchor(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z );
//            dJointSetUniversalAxis1(Vehicle.Axles[i].Hub[j], 0, 1, 0);
//            dJointSetUniversalAxis2(Vehicle.Axles[i].Hub[j], 1, 0, 0);

//            Vehicle.Axles[i].FourLink[j] = dJointCreateHinge(World, 0);
//            dJointAttach(Vehicle.Axles[i].FourLink[j], Vehicle.Body, Vehicle.Axles[i].Body);
//            dJointSetHingeAxis(Vehicle.Axles[i].FourLink[j], 0, 0, 1);
//
//            if (j == 0) {
//                dJointSetHingeAnchor(Vehicle.Axles[i].FourLink[j], GetAxleBarOffset().x, GetAxleBarOffset().y, GetAxleBarOffset().z);
//            } else {
//                dJointSetHingeAnchor(Vehicle.Axles[i].FourLink[j], -GetAxleBarOffset().x, GetAxleBarOffset().y, GetAxleBarOffset().z);
//            }

            Vehicle.Axles[i].FourLink[j] = dJointCreateBall(World, 0);
            dJointAttach(Vehicle.Axles[i].FourLink[j], Vehicle.Body, Vehicle.Axles[i].Body);

            if (j == 0) {
                dJointSetBallAnchor(Vehicle.Axles[i].FourLink[j], GetAxleBarOffset().x, GetAxleBarOffset().y, GetAxleBarOffset().z);
            } else {
                dJointSetBallAnchor(Vehicle.Axles[i].FourLink[j], -GetAxleBarOffset().x, GetAxleBarOffset().y, GetAxleBarOffset().z);
            }

            Vehicle.Axles[i].Wheels[j].ShockUpper[0] = dCreateSphere(VehicleSpace, 0.00001f);
            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockUpper[0], Vehicle.Body);
            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[0], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, Vehicle.Axles[i].Wheels[j].Position.z + 0.01f);

            Vehicle.Axles[i].Wheels[j].ShockUpper[1] = dCreateSphere(VehicleSpace, 0.00001f);
            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockUpper[1], Vehicle.Body);
            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[1], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, Vehicle.Axles[i].Wheels[j].Position.z - 0.01f);

            Vehicle.Axles[i].Wheels[j].ShockLower[0] = dCreateSphere(VehicleSpace, 0.00001f);
            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockLower[0], Vehicle.Axles[i].Body);
            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[0], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, +0.01f);

            Vehicle.Axles[i].Wheels[j].ShockLower[1] = dCreateSphere(VehicleSpace, 0.00001f);
            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockLower[1], Vehicle.Axles[i].Body);
            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[1], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, -0.01f);

            Vehicle.Axles[i].Motor[j] = dJointCreateHinge2(World, 0);
            dJointAttach(Vehicle.Axles[i].Motor[j], Vehicle.Body, Vehicle.Axles[i].Wheels[j].Body);

            dJointSetHinge2Anchor(Vehicle.Axles[i].Motor[j], Vehicle.Axles[i].Wheels[j].Position.x, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z);

            dJointSetHinge2Axis1(Vehicle.Axles[i].Motor[j], 0, 1, 0);
            dJointSetHinge2Axis2(Vehicle.Axles[i].Motor[j], 1, 0, 0);
        }
    }

    SetPhysics();

//    SetPosition(Center);
    SetOrientation(Center, Rotation);

    return true;

}

void CVehicle::SetPosition(glm::vec3 Centrum)
{

    this->Center = Centrum;

//    dMatrix3 R;
//    dRSetIdentity(R);

    dBodySetLinearVel(Vehicle.Body, 0, 0, 0);
    dBodySetAngularVel(Vehicle.Body, 0, 0, 0);

//    dBodySetRotation(Vehicle.Body, R);
    dBodySetPosition(Vehicle.Body, Center.x, Center.y, Center.z);

    for (int i = 0; i < 2; i++) {

        dBodySetLinearVel(Vehicle.Axles[i].Body, 0, 0, 0);
        dBodySetAngularVel(Vehicle.Axles[i].Body, 0, 0, 0);

//        dRSetIdentity(R);
//        dBodySetRotation(Vehicle.Axles[i].Body, R);
        dBodySetPosition(Vehicle.Axles[i].Body, Vehicle.Axles[i].Position.x + Center.x, Vehicle.Axles[i].Position.y + Center.y, Vehicle.Axles[i].Position.z + Center.z);

        for (int j = 0; j < 2; j++) {

            dBodySetLinearVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);
            dBodySetAngularVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);

//            dRSetIdentity(R);
//            dRFromEulerAngles(R, 0.0f, glm::radians(90.0f), 0.0f);
//            dBodySetRotation(Vehicle.Axles[i].Wheels[j].Body, R);
            dBodySetPosition(Vehicle.Axles[i].Wheels[j].Body, Vehicle.Axles[i].Wheels[j].Position.x + Center.x, Vehicle.Axles[i].Wheels[j].Position.y + Center.y, Vehicle.Axles[i].Wheels[j].Position.z + Center.z);

        }
    }

}

void CVehicle::SetOrientation(glm::vec3 Position, glm::vec3 Rotation)
{

//	dMatrix3 R, R1;
//	dRSetIdentity( R );

//    MTM: theta, phi, psi
//    ODE: phi, theta, psi

//    float Model[16];
//
//    glLoadIdentity( );
//    glPushMatrix( );
//    glRotatef( 270.0f - glm::degrees(Rotation[0]), 0, 1, 0);
//    glRotatef( -glm::degrees(Rotation[1]), 1, 0, 0 );
//    glRotatef( glm::degrees(Rotation[2]), 0, 0, 1 );
//    glGetFloatv( GL_MODELVIEW_MATRIX, Model );
//    glPopMatrix( );
//
//    dReal Pos[3];
//    Pos[0] = Position[0];
//    Pos[1] = Position[1];
//    Pos[2] = Position[2];
//
//    OGLtoODE( Model, Pos, R );

//    std::cout << glm::radians(270.0f) - Rotation.x << std::endl;
//
    dMatrix3 R;
    dRSetIdentity(R);
//    dRFromEulerAngles(R, -Rotation.y, glm::radians(270.0f) - Rotation.x, Rotation.z);

    dBodySetLinearVel(Vehicle.Body, 0, 0, 0);
    dBodySetAngularVel(Vehicle.Body, 0, 0, 0);

    dBodySetRotation(Vehicle.Body, R);
    dBodySetPosition(Vehicle.Body, Position.x, Position.y, Position.z);

    for (int i = 0; i < 2; i++) {

        dBodySetLinearVel(Vehicle.Axles[i].Body, 0, 0, 0);
        dBodySetAngularVel(Vehicle.Axles[i].Body, 0, 0, 0);

        dBodySetRotation(Vehicle.Axles[i].Body, R);
        dBodySetPosition(Vehicle.Axles[i].Body, Vehicle.Axles[i].Position.x + Position.x, Vehicle.Axles[i].Position.y + Position.y, Vehicle.Axles[i].Position.z + Position.z);


        for (int j = 0; j < 2; j++) {

            dBodySetLinearVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);
            dBodySetAngularVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);

            dMatrix3 R1;
            dRSetIdentity(R1);
            dRFromEulerAngles(R1, 0.0f, glm::radians(90.0f), 0);
            dBodySetRotation(Vehicle.Axles[i].Wheels[j].Body, R1);
            dBodySetPosition(Vehicle.Axles[i].Wheels[j].Body, Vehicle.Axles[i].Wheels[j].Position.x + Position.x, Vehicle.Axles[i].Wheels[j].Position.y + Position.y, Vehicle.Axles[i].Wheels[j].Position.z + Position.z);

        }
    }

}

void CVehicle::Reset(void)
{
    SetOrientation(Center, Rotation);

}

void CVehicle::Flip(void)
{
    glm::vec3 Position = GetPosition();
    Position.y += 0.25f;

    glm::vec3 Rotate = Rotation;
    Rotate.x = glm::radians(Config->Camera.Angle);

    SetOrientation(Position, Rotate);

}

bool CVehicle::Update(dReal Delta)
{

    dBodyEnable(Vehicle.Body);

    Velocity = GetVelocity();
    Transmission.Rpms = (Velocity * Transmission.MaxRpms / Transmission.GearNext[Transmission.Gear][2]);
    Transmission.Speed = (Velocity * 3600.0f / 1000.0f);

    if (Transmission.Gear > 0) {
        AutomaticGear();
        CheckGear(Delta);
    } else {
        Transmission.Speed = 0.0f;
        Transmission.Rpms = 0.0f;
    }

    if (((glfwGetKey(GLWindow, Config->Input.DownShift) == GLFW_PRESS)) && (Transmission.Gear > 0)) {
        Transmission.Gear--;
    }

    if (((glfwGetKey(GLWindow, Config->Input.UpShift) == GLFW_PRESS)) && (Transmission.Gear < Transmission.GearCount)) {
        Transmission.Gear++;
    }

    if(((glfwGetKey(GLWindow, Config->Input.DownShift) == GLFW_RELEASE) || Input->GetButtonState(Config->Joypad.DownShift)) && (Transmission.Gear > 0)) {
        Transmission.Gear--;
    }

    if(((glfwGetKey(GLWindow, Config->Input.UpShift) == GLFW_RELEASE) || Input->GetButtonState(Config->Joypad.UpShift)) && (Transmission.Gear < Transmission.GearCount)) {
        Transmission.Gear++;
    }

    if (Input->GetButtonState(Config->Joypad.Reset)) {
        Reset();
    }

    if (Input->GetButtonState(Config->Joypad.Flip)) {
        Flip();
    }

    if(glfwGetKey(GLWindow, Config->Input.Gas) == GLFW_PRESS || Input->GetButtonState(Config->Joypad.Gas)) {

        Vehicle.Speed = Transmission.Speed + 1.0f;
        Vehicle.WheelForce = Transmission.Acceleration;

    } else if(glfwGetKey(GLWindow, Config->Input.Brake) == GLFW_PRESS || Input->GetButtonState(Config->Joypad.Brake)) {

        Vehicle.Speed = -Transmission.Speed;
        Vehicle.WheelForce = Transmission.Acceleration;

    } else {

        Vehicle.Speed = 0.0f;
        Vehicle.WheelForce = 0.0f;
    }


    float Scaling = 5.0f;

    if(glfwGetKey(GLWindow, Config->Input.FrontSteerRight) || (Input->GetAxisState(abs(Config->Joypad.FrontSteerRight)) > 0.25f)) {
        Vehicle.Axles[0].SteerAngle += Vehicle.Axles[0].SteerSpeed * Delta;
        Scaling = 5;
    } else if(glfwGetKey(GLWindow, Config->Input.FrontSteerLeft) || (Input->GetAxisState(abs(Config->Joypad.FrontSteerLeft)) < -0.25f)) {
        Vehicle.Axles[0].SteerAngle -= Vehicle.Axles[0].SteerSpeed * Delta;
        Scaling = -5;
    } else {
        if(Vehicle.Axles[0].SteerAngle < -0.01f) {
            Vehicle.Axles[0].SteerAngle += Vehicle.Axles[0].CenterSpeed * Delta * Scaling;
        } else if(Vehicle.Axles[0].SteerAngle > 0.01f) {
            Vehicle.Axles[0].SteerAngle -= Vehicle.Axles[0].CenterSpeed * Delta * Scaling;
        } else {
            Vehicle.Axles[0].SteerAngle = 0.0f;
        }
    }

    Vehicle.Axles[0].SteerAngle = glm::clamp(Vehicle.Axles[0].SteerAngle, Vehicle.Axles[0].MinSteer, Vehicle.Axles[0].MaxSteer);

    if(glfwGetKey(GLWindow, Config->Input.RearSteerRight) || (Input->GetAxisState(abs(Config->Joypad.RearSteerRight)) > 0.25f)) {
        Vehicle.Axles[1].SteerAngle += Vehicle.Axles[1].SteerSpeed * Delta;
        Scaling = 5;
    } else if(glfwGetKey(GLWindow, Config->Input.RearSteerLeft) || (Input->GetAxisState(abs(Config->Joypad.RearSteerLeft)) < -0.25f)) {
        Vehicle.Axles[1].SteerAngle -= Vehicle.Axles[1].SteerSpeed * Delta;
        Scaling = -5;
    } else {
        if(Vehicle.Axles[1].SteerAngle < -0.01f) {
            Vehicle.Axles[1].SteerAngle += Vehicle.Axles[1].CenterSpeed * Delta * Scaling;
        } else if(Vehicle.Axles[1].SteerAngle > 0.01f) {
            Vehicle.Axles[1].SteerAngle -= Vehicle.Axles[1].CenterSpeed * Delta * Scaling;
        } else {
            Vehicle.Axles[1].SteerAngle = 0.0f;
        }
    }

    Vehicle.Axles[1].SteerAngle = glm::clamp(Vehicle.Axles[1].SteerAngle, Vehicle.Axles[1].MinSteer, Vehicle.Axles[1].MaxSteer);

    dVector3 Axis;

    for (unsigned int i = 0; i < 2; i++) {

        for (unsigned int j = 0; j < 2; j++) {

//            Vehicle.Axles[i].Wheels[j].Data.Slip = (Transmission.MaxSpeed - Vehicle.Speed) * 0.000001f;

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamLoStop, Vehicle.Axles[i].SteerAngle * Vehicle.Axles[i].Wheels[j].SteerMultiplier);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamHiStop, Vehicle.Axles[i].SteerAngle * Vehicle.Axles[i].Wheels[j].SteerMultiplier);

            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamVel2, Vehicle.Speed * Vehicle.Axles[i].Wheels[j].DriveMultiplier);
            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFMax2, Vehicle.WheelForce * Vehicle.Axles[i].Wheels[j].DriveMultiplier);

            dJointGetHinge2Axis2(Vehicle.Axles[i].Motor[j], Axis);
            dBodySetFiniteRotationAxis(Vehicle.Axles[i].Wheels[j].Body, Axis[0], Axis[1], Axis[2]);

        }

    }


//    glm::vec3 VehiclePos = GetPosition();
//
//    if (VehiclePos.x < 0) {
//        VehiclePos.x += Boundaries;
//        SetPosition(VehiclePos);
//    }
//
//    if (VehiclePos.z < 0) {
//        VehiclePos.z += Boundaries;
//        SetPosition(VehiclePos);
//    }
//
//    if (VehiclePos.x > Boundaries) {
//        VehiclePos.x -= Boundaries;
//        SetPosition(VehiclePos);
//    }
//
//    if (VehiclePos.z > Boundaries) {
//        VehiclePos.z -= Boundaries;
//        SetPosition(VehiclePos);
//    }

    return true;

}

void CVehicle::Render(void)
{
    double Matrix[16];

    const dReal *Position = dBodyGetPosition(Vehicle.Body);
    const dReal *Rotation = dBodyGetRotation(Vehicle.Body);

    ODEtoOGL(Matrix, Position, Rotation);

    glPushMatrix();

    glMultMatrixd(Matrix);
    glCallList(GetBody().GetModel());

    glPopMatrix();

    dVector3 DriveShaftPosition;
    dVector3 HubPosition;
    dVector3 FourLinkPosition;

    for(unsigned int i = 0; i < 2; i++) {

        const dReal *AxlePosition = dBodyGetPosition(Vehicle.Axles[i].Body);
        dJointGetBallAnchor(Vehicle.Axles[i].DriveShaft, DriveShaftPosition);
//        dJointGetHingeAnchor(Vehicle.Axles[i].DriveShaft, DriveShaftPosition);

        GetDriveShaftTexture().Bind(0);
        Cylinder(glm::vec3(AxlePosition[0], AxlePosition[1], AxlePosition[2]), glm::vec3(DriveShaftPosition[0], DriveShaftPosition[1], DriveShaftPosition[2]), 0.006125f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);

        const dReal *Position = dBodyGetPosition(Vehicle.Axles[i].Body);
        const dReal *Rotation = dBodyGetRotation(Vehicle.Axles[i].Body);

        ODEtoOGL(Matrix, Position, Rotation);

        glPushMatrix();

        glMultMatrixd(Matrix);

        glCallList(GetAxle().GetModel());

        glPopMatrix();

        for(unsigned int j = 0; j < 2; j++) {

            dJointGetBallAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
//            dJointGetHingeAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
//            dJointGetUniversalAnchor(Vehicle.Axles[i].Hub[j], HubPosition);

            dJointGetBallAnchor(Vehicle.Axles[i].FourLink[j], FourLinkPosition);
//            dJointGetHingeAnchor(Vehicle.Axles[i].FourLink[j], FourLinkPosition);

            GetBarTexture().Bind(0);
            Cylinder(glm::vec3(FourLinkPosition[0], FourLinkPosition[1], FourLinkPosition[2]), glm::vec3(HubPosition[0], HubPosition[1], HubPosition[2]), 0.006125f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);

            const dReal *ShockPositionUpper1 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[0]);
            const dReal *ShockPositionUpper2 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[1]);
            const dReal *ShockPositionLower1 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[0]);
            const dReal *ShockPositionLower2 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[1]);

            GetShockTexture().Bind(0);
            Cylinder(glm::vec3(ShockPositionUpper1[0], ShockPositionUpper1[1], ShockPositionUpper1[2]), glm::vec3(ShockPositionLower1[0], ShockPositionLower1[1], ShockPositionLower1[2]), 0.0030625f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);
            Cylinder(glm::vec3(ShockPositionUpper2[0], ShockPositionUpper2[1], ShockPositionUpper2[2]), glm::vec3(ShockPositionLower2[0], ShockPositionLower2[1], ShockPositionLower2[2]), 0.0030625f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);

            const dReal *Position = dBodyGetPosition(Vehicle.Axles[i].Wheels[j].Body);
            const dReal *Rotation = dBodyGetRotation(Vehicle.Axles[i].Wheels[j].Body);

            ODEtoOGL(Matrix, Position, Rotation);

            glPushMatrix();

            glMultMatrixd(Matrix);

            glRotatef(90.0f, 0, 1, 0);
            glCallList(GetWheel(j).GetModel());

            glPopMatrix();

        }

    }

//    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//    glPointSize(14);
//    glBegin(GL_POINTS);
//
//    for ( int i = 0; i < 12; i++ ) {
//
//        const dReal *Position = dGeomGetPosition(CollisionPoints[i]);
//        const dReal *Rotation = dGeomGetRotation(CollisionPoints[i]);
//
//        ODEtoOGL(Matrix, Position, Rotation);
//
//        glPushMatrix();
//
//            glMultMatrixd(Matrix);
//
//            glVertex3f(Position[0], Position[1], Position[2]);
//
//
//        glPopMatrix();
//
//    }
//    glEnd();

}
void CVehicle::Cylinder(glm::vec3 Start, glm::vec3 End, float Radius, int Subdivisions, glm::vec4 Color, bool Lighted)
{

    glDisable(GL_LIGHTING);
    glColor4f(Color.r, Color.g, Color.b, Color.a);

    glm::vec3 Vector = End - Start;


    float V = glm::length(End - Start);
    float Ax = 57.2957795 * glm::acos(Vector.z / V);

    if (Vector.z < 0.0) {
        Ax = -Ax;
    }

    float RotateX = -Vector.y * Vector.z;
    float RotateY = Vector.x * Vector.z;

    float X = 0.0f;
    float Y = 0.0f;

    glPushMatrix();

    glTranslatef(Start.x, Start.y, Start.z);
    glRotatef(Ax, RotateX, RotateY, 0.0f);

    glBegin(GL_QUAD_STRIP);

    for (int i = 0; i <= Subdivisions; i++) {

        double U = i / (double)Subdivisions;

        X = Radius * cos(2 * 3.1415926f * U);
        Y = Radius * sin(2 * 3.1415926f * U);

        glTexCoord2d(U, 1.0f);
        glVertex3f(X, Y, V);
        glTexCoord2d(U, 0.0f);
        glVertex3f(X, Y, 0);

    }

    glEnd();

    glPopMatrix();

    if (Lighted) {
        glEnable(GL_LIGHTING);
    }

}
//void CVehicle::RenderParts(glm::mat4 LookAt)
//{
//    glUseProgram(0);
//    glLoadMatrixf(glm::value_ptr(LookAt));
//
//    dVector3 DriveShaftPosition;
//    dVector3 HubPosition;
//    dVector3 FourLinkPosition;
//
//    for (unsigned int i = 0; i < 2; i++) {
//
//        const dReal *AxlePosition = dBodyGetPosition(Vehicle.Axles[i].Body);
//        dJointGetBallAnchor(Vehicle.Axles[i].DriveShaft, DriveShaftPosition);
//
//        GetDriveShaftTexture().Bind(0);
//        Cylinder(glm::vec3(AxlePosition[0], AxlePosition[1], AxlePosition[2]), glm::vec3(DriveShaftPosition[0], DriveShaftPosition[1], DriveShaftPosition[2]), 0.006125f, 6, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
//
//        for (unsigned int j = 0; j < 2; j++) {
//
////            dJointGetUniversalAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
//            dJointGetBallAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
////            dJointGetHingeAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
//
////            dJointGetHingeAnchor(Vehicle.Axles[i].FourLink[j], FourLinkPosition);
//            dJointGetBallAnchor(Vehicle.Axles[i].FourLink[j], FourLinkPosition);
//
//
//            GetBarTexture().Bind(0);
//            Cylinder(glm::vec3(FourLinkPosition[0], FourLinkPosition[1], FourLinkPosition[2]), glm::vec3(HubPosition[0], HubPosition[1], HubPosition[2]), 0.006125f, 6, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
//
//            const dReal *ShockPositionUpper1 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[0]);
//            const dReal *ShockPositionUpper2 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[1]);
//            const dReal *ShockPositionLower1 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[0]);
//            const dReal *ShockPositionLower2 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[1]);
//
//            GetShockTexture().Bind(0);
//            Cylinder(glm::vec3(ShockPositionUpper1[0], ShockPositionUpper1[1], ShockPositionUpper1[2]), glm::vec3(ShockPositionLower1[0], ShockPositionLower1[1], ShockPositionLower1[2]), 0.0030625f, 6, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
//            Cylinder(glm::vec3(ShockPositionUpper2[0], ShockPositionUpper2[1], ShockPositionUpper2[2]), glm::vec3(ShockPositionLower2[0], ShockPositionLower2[1], ShockPositionLower2[2]), 0.0030625f, 6, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
//        }
//    }

//    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
//    glPointSize(6);
//    glBegin(GL_POINTS);
//
//    float Matrix[16];
//
//    for ( int i = 0; i < 12; i++ ) {
//
//        const dReal *Position = dGeomGetPosition(CollisionPoints[i]);
//        const dReal *Rotation = dGeomGetRotation(CollisionPoints[i]);
//
//        ODEtoOGL(Matrix, Position, Rotation);
//
//        glPushMatrix();
//
//            glMultMatrixf(Matrix);
//
//            glVertex3f(Position[0], Position[1], Position[2]);
//
//
//        glPopMatrix();
//
//    }
//    glEnd();
//
//}

float CVehicle::GetVelocity(void)
{
    dReal const *CV = dBodyGetLinearVel(Vehicle.Body);
    float V2 = (CV[0] * CV[0] +/* CV[1] * CV[1] +*/ CV[2] * CV[2]);
    float V = sqrtf(V2) / 1000 * 3600;

    return  V;
}

//
//
//#include "Vehicle.hpp"
//
//using namespace Physics;
//
//double CVehicle::DownShift(unsigned int Index)
//{
//    return -Transmission.GearAccel[Index][1] / (Transmission.GearNext[Index][2] - Transmission.GearNext[Index][1]);
//}
//
//double CVehicle::UpShift(unsigned int Index)
//{
//    return (Transmission.GearAccel[Index][1] - Transmission.GearAccel[Index][0]) / Transmission.GearNext[Index][1];
//}
//
//double CVehicle::BestGear(unsigned int Index)
//{
//    if((Index < (unsigned int)Transmission.GearCount) && (Index > 0)) {
//        return (Transmission.GearAccel[Index + 1][0] - Transmission.GearAccel[Index][1] + DownShift(Index) * Transmission.GearNext[Index][1]) / (DownShift(Index) - UpShift(Index + 1));
//    }
//
//    if(Index == (unsigned int)Transmission.GearCount) {
//        return Transmission.GearNext[Transmission.GearCount][2];
//    }
//
//    return 0;
//}
//
//void CVehicle::CheckGear(double Delta)
//{
//    unsigned int Index = 0;
//
//    for(unsigned int i = 0; i < 3; i++) {
//        if(Transmission.Speed > Transmission.GearNext[Transmission.Gear][i]) {
//            Index = i;
//        }
//    }
//
//    if(Index == 3) {
//        Transmission.Speed += Delta * -Transmission.Drag * 3600 / 1000;
//    } else {
//        double Value = (Transmission.GearNext[Transmission.Gear][Index + 1] - Transmission.GearNext[Transmission.Gear][Index]) / (Transmission.GearAccel[Transmission.Gear][Index + 1] - Transmission.GearAccel[Transmission.Gear][Index]);
//        Transmission.Acceleration = (Transmission.Speed - Transmission.GearNext[Transmission.Gear][Index]) / Value + Transmission.GearAccel[Transmission.Gear][Index];
//    }
//
////    if (Transmission.Speed > Transmission.MaxSpeed) {
////        Transmission.Speed = Transmission.MaxSpeed;
////    }
//
//    Transmission.Rpms = Transmission.Speed * Transmission.MaxRpms / Transmission.GearNext[Transmission.Gear][2];
//
////    if (Transmission.Rpms > Transmission.MaxRpms) {
////        Transmission.Rpms = Transmission.MaxRpms;
////    }
//
////    std::cout << Transmission.Speed << ", " << Transmission.Rpms << std::endl;
//
//}
//
//void CVehicle::AutomaticGear(void)
//{
//    if(!Transmission.Automatic) {
//        return;
//    }
//
//    if(Transmission.Speed >= BestGear(Transmission.Gear) && (Transmission.Gear < Transmission.GearCount)) {
//        Transmission.Gear++;
//    }
//
//    if((Transmission.Gear > 1) && (Transmission.Speed < BestGear(Transmission.Gear - 1))) {
//        Transmission.Gear--;
//    }
//}
//
//CVehicle::CVehicle(void) : Velocity(0.0f)
//{
//    Vehicle.Body = NULL;
//
//    for(int i = 0; i < 12; i++) {
//        CollisionPoints[i] = NULL;
//    }
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        Vehicle.Axles[i].Body = NULL;
//        Vehicle.Axles[i].Geom = NULL;
//        Vehicle.Axles[i].DriveShaft = NULL;
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            for(unsigned int k = 0; k < 2; k++) {
//
//                Vehicle.Axles[i].Wheels[j].ShockLower[k] = NULL;
//                Vehicle.Axles[i].Wheels[j].ShockUpper[k] = NULL;
//
//            }
//
//            Vehicle.Axles[i].Wheels[j].Body = NULL;
//            Vehicle.Axles[i].Wheels[j].Geom = NULL;
//            Vehicle.Axles[i].Motor[j] = NULL;
//            Vehicle.Axles[i].Hub[j] = NULL;
//            Vehicle.Axles[i].FourLink[j] = NULL;
//
//        }
//    }
//
//    VehicleSpace = NULL;
//
//}
//
//CVehicle::~CVehicle(void)
//{
//    Destroy();
//}
//
//void CVehicle::Destroy(void)
//{
//
//    if (Config->Physics.Save) {
//        SavePhysics(Config->Physics.VehicleFile);
//    }
//
//    Center = glm::vec3(0.0f);
//
//    if(Vehicle.Body) {
//        dBodyDestroy(Vehicle.Body);
//        Vehicle.Body = NULL;
//    }
//
//    for(int i = 0; i < 12; i++) {
//        dGeomDestroy(CollisionPoints[i]);
//        CollisionPoints[i] = NULL;
//    }
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        if(Vehicle.Axles[i].Body) {
//            dBodyDestroy(Vehicle.Axles[i].Body);
//            Vehicle.Axles[i].Body = NULL;
//        }
//
//        if(Vehicle.Axles[i].Geom) {
//            dGeomDestroy(Vehicle.Axles[i].Geom);
//            Vehicle.Axles[i].Geom = NULL;
//        }
//
//        if(Vehicle.Axles[i].DriveShaft) {
//            dJointDestroy(Vehicle.Axles[i].DriveShaft);
//            Vehicle.Axles[i].DriveShaft = NULL;
//        }
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            for(unsigned int k = 0; k < 2; k++) {
//
//                if(Vehicle.Axles[i].Wheels[j].ShockLower[k]) {
//                    dGeomDestroy(Vehicle.Axles[i].Wheels[j].ShockLower[k]);
//                    Vehicle.Axles[i].Wheels[j].ShockLower[k] = NULL;
//                }
//
//                if(Vehicle.Axles[i].Wheels[j].ShockUpper[k]) {
//                    dGeomDestroy(Vehicle.Axles[i].Wheels[j].ShockUpper[k]);
//                    Vehicle.Axles[i].Wheels[j].ShockUpper[k] = NULL;
//                }
//            }
//
//            if(Vehicle.Axles[i].Wheels[j].Body) {
//                dBodyDestroy(Vehicle.Axles[i].Wheels[j].Body);
//                Vehicle.Axles[i].Wheels[j].Body = NULL;
//            }
//
//            if(Vehicle.Axles[i].Wheels[j].Geom) {
//                dGeomDestroy(Vehicle.Axles[i].Wheels[j].Geom);
//                Vehicle.Axles[i].Wheels[j].Geom = NULL;
//            }
//
//            if(Vehicle.Axles[i].Motor[j]) {
//                dJointDestroy(Vehicle.Axles[i].Motor[j]);
//                Vehicle.Axles[i].Motor[j] = NULL;
//            }
//
//            if(Vehicle.Axles[i].Hub[j]) {
//                dJointDestroy(Vehicle.Axles[i].Hub[j]);
//                Vehicle.Axles[i].Hub[j] = NULL;
//            }
//
//            if(Vehicle.Axles[i].FourLink[j]) {
//                dJointDestroy(Vehicle.Axles[i].FourLink[j]);
//                Vehicle.Axles[i].FourLink[j] = NULL;
//            }
//
//        }
//    }
//
//    if(VehicleSpace) {
//        dSpaceDestroy(VehicleSpace);
//        VehicleSpace = NULL;
//    }
//}
//
//bool CVehicle::Defaults(std::string Filename)
//{
//
//    ZeroMemory(&Vehicle, sizeof(Vehicle));
//
//    Vehicle.MassScale = 1.0f;
//    Vehicle.Mass = 7600;
//    Vehicle.COGMass = 1000;
//    Vehicle.Data.Slip = 0.000001f;
//
//    Vehicle.Axles[0].Mass = 800;
//    Vehicle.Axles[0].Data.Slip = 0.000001f;
//    Vehicle.Axles[0].MinSteer = -0.35f;
//    Vehicle.Axles[0].MaxSteer = 0.35f;
//    Vehicle.Axles[0].SteerSpeed = 0.5f;
//
//    Vehicle.Axles[0].Wheels[0].DriveMultiplier = 1;
//    Vehicle.Axles[0].Wheels[0].SteerMultiplier = 1;
//    Vehicle.Axles[0].Wheels[0].Data.Slip = 0.000001f;
//    Vehicle.Axles[0].Wheels[0].Damping = 0.000028f;
//    Vehicle.Axles[0].Wheels[0].Error = 0.6f;
//    Vehicle.Axles[0].Wheels[0].StopERP = 0.6f;
//    Vehicle.Axles[0].Wheels[0].StopCFM = 0.000001f;
//    Vehicle.Axles[0].Wheels[0].Bounce = 0.02f;
//    Vehicle.Axles[0].Wheels[0].CFM = 0.000001f;
//    Vehicle.Axles[0].Wheels[0].FMax = 10.0f;
//    Vehicle.Axles[0].Wheels[0].FudgeFactor = 0.8f;
//    Vehicle.Axles[0].Wheels[0].Mass = 750;
//
//    Vehicle.Axles[0].Wheels[1].DriveMultiplier = 1;
//    Vehicle.Axles[0].Wheels[1].SteerMultiplier = 1;
//    Vehicle.Axles[0].Wheels[1].Data.Slip = 0.000001f;
//    Vehicle.Axles[0].Wheels[1].Damping = 0.000028f;
//    Vehicle.Axles[0].Wheels[1].Error = 0.6f;
//    Vehicle.Axles[0].Wheels[1].StopERP = 0.6f;
//    Vehicle.Axles[0].Wheels[1].StopCFM = 0.000001f;
//    Vehicle.Axles[0].Wheels[1].Bounce = 0.02f;
//    Vehicle.Axles[0].Wheels[1].CFM = 0.000001f;
//    Vehicle.Axles[0].Wheels[1].FMax = 10.0f;
//    Vehicle.Axles[0].Wheels[1].FudgeFactor = 0.8f;
//    Vehicle.Axles[0].Wheels[1].Mass = 750;
//
//    Vehicle.Axles[1].Mass = 800;
//    Vehicle.Axles[1].Data.Slip = 0.000001f;
//    Vehicle.Axles[1].MinSteer = -0.35f;
//    Vehicle.Axles[1].MaxSteer = 0.35f;
//    Vehicle.Axles[1].SteerSpeed = 0.5f;
//
//    Vehicle.Axles[1].Wheels[0].DriveMultiplier = 1;
//    Vehicle.Axles[1].Wheels[0].SteerMultiplier = -1;
//    Vehicle.Axles[1].Wheels[0].Data.Slip = 0.000001f;
//    Vehicle.Axles[1].Wheels[0].Damping = 0.000028f;
//    Vehicle.Axles[1].Wheels[0].Error = 0.6f;
//    Vehicle.Axles[1].Wheels[0].StopERP = 0.6f;
//    Vehicle.Axles[1].Wheels[0].StopCFM = 0.000001f;
//    Vehicle.Axles[1].Wheels[0].Bounce = 0.02f;
//    Vehicle.Axles[1].Wheels[0].CFM = 0.000001f;
//    Vehicle.Axles[1].Wheels[0].FMax = 10.0f;
//    Vehicle.Axles[1].Wheels[0].FudgeFactor = 0.8f;
//    Vehicle.Axles[1].Wheels[0].Mass = 750;
//
//    Vehicle.Axles[1].Wheels[1].DriveMultiplier = 1;
//    Vehicle.Axles[1].Wheels[1].SteerMultiplier = -1;
//    Vehicle.Axles[1].Wheels[1].Data.Slip = 0.000001f;
//    Vehicle.Axles[1].Wheels[1].Damping = 0.000028f;
//    Vehicle.Axles[1].Wheels[1].Error = 0.6f;
//    Vehicle.Axles[1].Wheels[1].StopERP = 0.6f;
//    Vehicle.Axles[1].Wheels[1].StopCFM = 0.000001f;
//    Vehicle.Axles[1].Wheels[1].Bounce = 0.02f;
//    Vehicle.Axles[1].Wheels[1].CFM = 0.000001f;
//    Vehicle.Axles[1].Wheels[1].FMax = 10.0f;
//    Vehicle.Axles[1].Wheels[1].FudgeFactor = 0.8f;
//    Vehicle.Axles[1].Wheels[1].Mass = 750;
//
//    ZeroMemory(&Transmission, sizeof(Transmission));
//
//    Transmission.Automatic = true;
//    Transmission.Gear = 1;
//    Transmission.GearCount = 2;
//    Transmission.Speed = 0.0f;
//    Transmission.Rpms = 0.0f;
//    Transmission.MaxRpms = 8500.0f;
//    Transmission.Brake = -100;
//    Transmission.MaxSpeed = 65.0f;
//    Transmission.ReverseSpeed = -25.0f;
//    Transmission.Drag = -10.0f;
//    Transmission.HorsePower = 1500.0f;
//    Transmission.Acceleration = 1.0f;
//
//    float Scale = Transmission.HorsePower;
//    Transmission.GearAccel[0][0] = 1 * Scale;
//    Transmission.GearAccel[1][0] = 3.059000 * Transmission.GearAccel[0][0];
//    Transmission.GearAccel[2][0] = 1.625000 * Transmission.GearAccel[0][0];;
//
//    Transmission.GearAccel[0][1] = 2 * Scale;
//    Transmission.GearAccel[1][1] = 1.100000 * Transmission.GearAccel[0][1];
//    Transmission.GearAccel[2][1] = 0.900000 * Transmission.GearAccel[0][1];
//
//    Transmission.GearAccel[0][2] = 0.0f;
//    Transmission.GearAccel[1][2] = 0.0f;
//    Transmission.GearAccel[2][2] = 0.0f;
//
//    Transmission.GearNext[0][0] = 0.0f;
//    Transmission.GearNext[1][0] = 0.0f;
//    Transmission.GearNext[2][0] = 0.0f;
//
//    Scale = 1.0f;
//    Transmission.GearNext[0][1] = 0.0f * Scale;
//    Transmission.GearNext[1][1] = 15.0f * Scale;
//    Transmission.GearNext[2][1] = 35.0f * Scale;
//
//    Transmission.GearNext[0][2] = 10.0f * Scale;
//    Transmission.GearNext[1][2] = 45.0f * Scale;
//    Transmission.GearNext[2][2] = 65.0f * Scale;
//
//    SavePhysics(Filename);
//
//    return true;
//
//}
//
//void CVehicle::SetPhysics(void)
//{
//    dMass M;
//    glm::vec3 Min, Max, Dimensions;
//
//    Dimensions = GetBody().GetBounds(Min, Max);
//
//    dMassSetBoxTotal(&M, Vehicle.Mass * Vehicle.MassScale, Dimensions.x, Dimensions.y, Dimensions.z);
//    dBodySetMass(Vehicle.Body, &M);
//
////    for (int i = 0; i < 12; i++) {
////        dGeomSetData(CollisionPoints[i], (void*)&Vehicle.Data);
////    }
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        Dimensions = GetAxle().GetBounds(Min, Max);
//
//        dGeomSetData(Vehicle.Axles[i].Geom, (void*)&Vehicle.Axles[i].Data);
//
//        dMassSetCylinderTotal(&M, Vehicle.Axles[i].Mass * Vehicle.MassScale, 1, Max.y, Dimensions.x);
//        dBodySetMass(Vehicle.Axles[i].Body, &M);
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            Dimensions = GetWheel(j).GetBounds(Min, Max);
//
//            dGeomSetData(Vehicle.Axles[i].Wheels[j].Geom, (void*)&Vehicle.Axles[i].Wheels[j].Data);
//
//            dMassSetCylinderTotal(&M, Vehicle.Axles[i].Wheels[j].Mass * Vehicle.MassScale, 1, Max.y, Dimensions.x);
//            dBodySetMass(Vehicle.Axles[i].Wheels[j].Body, &M);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamLoStop, 0);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamHiStop, 0);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionERP, Vehicle.Axles[i].Wheels[j].Error);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionCFM, Vehicle.Axles[i].Wheels[j].Damping);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopERP, Vehicle.Axles[i].Wheels[j].Error);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopCFM, Vehicle.Axles[i].Wheels[j].StopCFM);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamBounce, Vehicle.Axles[i].Wheels[j].Bounce);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamCFM, Vehicle.Axles[i].Wheels[j].CFM);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFMax, Vehicle.Axles[i].Wheels[j].FMax);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFudgeFactor, Vehicle.Axles[i].Wheels[j].FudgeFactor);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamLoStop2, 0);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamHiStop2, 0);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionERP2, Vehicle.Axles[i].Wheels[j].Error);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamSuspensionCFM2, Vehicle.Axles[i].Wheels[j].Damping);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopERP2, Vehicle.Axles[i].Wheels[j].Error);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamStopCFM2, Vehicle.Axles[i].Wheels[j].StopCFM);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamBounce2, Vehicle.Axles[i].Wheels[j].Bounce);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamCFM2, Vehicle.Axles[i].Wheels[j].CFM);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFMax2, Vehicle.Axles[i].Wheels[j].FMax);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFudgeFactor2, Vehicle.Axles[i].Wheels[j].FudgeFactor);
//        }
//    }
//
//}
//
//void CVehicle::Cylinder(glm::vec3 Start, glm::vec3 End, float Radius, int Subdivisions, glm::vec4 Color, bool Lighted)
//{
//
////    if (!Lighted) {
//        glDisable(GL_LIGHTING);
////    }
//    glColor4f(Color.r, Color.g, Color.b, Color.a);
//
//    glm::vec3 Vector = End - Start;
//
//    if (Vector.z == 0) {
//        Vector.z = glm::epsilon<float>();
//    }
//
//    float V = glm::length(End - Start);
//    float Ax = 57.2957795 * glm::acos(Vector.z / V);
//
//    if(Vector.z < 0.0) {
//        Ax = -Ax;
//    }
//
//    float RotateX = -Vector.y * Vector.z;
//    float RotateY = Vector.x * Vector.z;
//
//    float X = 0.0f;
//    float Y = 0.0f;
//
//    glPushMatrix();
//
//    glTranslatef(Start.x, Start.y, Start.z);
//    glRotatef(Ax, RotateX, RotateY, 0.0f);
//
//    glBegin(GL_QUAD_STRIP);
//
//    for(int i = 0; i <= Subdivisions; i++) {
//
//        double U = i / (double)Subdivisions;
//
//        X = Radius * glm::cos(2 * glm::pi<float>() * U);
//        Y = Radius * glm::sin(2 * glm::pi<float>() * U);
//
//        glTexCoord2d(U, 1.0f);
//        glVertex3f(X, Y, V);
//        glTexCoord2d(U, 0.0f);
//        glVertex3f(X, Y, 0);
//
//    }
//
//    glEnd();
//
//    glPopMatrix();
//
//    if(Lighted) {
//        glEnable(GL_LIGHTING);
//    }
//
//}
//
//bool CVehicle::LoadPhysics(std::string Filename)
//{
//    std::string PhysicsFile = RootDir + ResourceDir + "Physics\\" + Filename;
//
//    if(Config->Physics.Defaults) {
//        Defaults(Filename);
//    }
//
//    std::ifstream CfgFile(PhysicsFile.c_str(), std::ios::in | std::ios::binary);
//
//    if(CfgFile.fail()) {
//        Logger->Error("Error Loading Vehicle Physics, Loading Defaults...", true, true, false);
//        Defaults(Filename);
//        return true;
//    }
//
//    CfgFile.read((char*)&Vehicle.MassScale, sizeof(Vehicle.MassScale));
//    CfgFile.read((char*)&Vehicle.Mass, sizeof(Vehicle.Mass));
//    CfgFile.read((char*)&Vehicle.COGMass, sizeof(Vehicle.COGMass));
//    CfgFile.read((char*)&Vehicle.Data.Slip, sizeof(Vehicle.Data.Slip));
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        CfgFile.read((char*)&Vehicle.Axles[i].Mass, sizeof(Vehicle.Axles[i].Mass));
//        CfgFile.read((char*)&Vehicle.Axles[i].MinSteer, sizeof(Vehicle.Axles[i].MinSteer));
//        CfgFile.read((char*)&Vehicle.Axles[i].MaxSteer, sizeof(Vehicle.Axles[i].MaxSteer));
//        CfgFile.read((char*)&Vehicle.Axles[i].SteerSpeed, sizeof(Vehicle.Axles[i].SteerSpeed));
//        CfgFile.read((char*)&Vehicle.Axles[i].Data.Slip, sizeof(Vehicle.Axles[i].Data.Slip));
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Mass, sizeof(Vehicle.Axles[i].Wheels[j].Mass));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].SteerMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].SteerMultiplier));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].DriveMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].DriveMultiplier));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Damping, sizeof(Vehicle.Axles[i].Wheels[j].Damping));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Error, sizeof(Vehicle.Axles[i].Wheels[j].Error));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].StopERP, sizeof(Vehicle.Axles[i].Wheels[j].StopERP));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].StopCFM, sizeof(Vehicle.Axles[i].Wheels[j].StopCFM));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Bounce, sizeof(Vehicle.Axles[i].Wheels[j].Bounce));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].CFM, sizeof(Vehicle.Axles[i].Wheels[j].CFM));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].FMax, sizeof(Vehicle.Axles[i].Wheels[j].FMax));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].FudgeFactor, sizeof(Vehicle.Axles[i].Wheels[j].FudgeFactor));
//            CfgFile.read((char*)&Vehicle.Axles[i].Wheels[j].Data.Slip, sizeof(Vehicle.Axles[i].Wheels[j].Data.Slip));
//        }
//    }
//
//    CfgFile.read((char*)&Transmission, sizeof(Transmission));
//
//    CfgFile.close();
//
//    return true;
//}
//
//bool CVehicle::SavePhysics(std::string Filename)
//{
//    std::string PhysicsFile = RootDir + ResourceDir + "Physics\\" + Filename;
//
//    std::ofstream CfgFile(PhysicsFile.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
//
//    if(CfgFile.fail()) {
//        Logger->Error("Error Saving Vehicle Physics, Aborting...", true, true, false);
//        return false;
//    }
//
//    CfgFile.write((char*)&Vehicle.MassScale, sizeof(Vehicle.MassScale));
//    CfgFile.write((char*)&Vehicle.Mass, sizeof(Vehicle.Mass));
//    CfgFile.write((char*)&Vehicle.COGMass, sizeof(Vehicle.COGMass));
//    CfgFile.write((char*)&Vehicle.Data.Slip, sizeof(Vehicle.Data.Slip));
//
//    for(unsigned int i = 0; i < 2; i++) {
//        CfgFile.write((char*)&Vehicle.Axles[i].Mass, sizeof(Vehicle.Axles[i].Mass));
//        CfgFile.write((char*)&Vehicle.Axles[i].MinSteer, sizeof(Vehicle.Axles[i].MinSteer));
//        CfgFile.write((char*)&Vehicle.Axles[i].MaxSteer, sizeof(Vehicle.Axles[i].MaxSteer));
//        CfgFile.write((char*)&Vehicle.Axles[i].SteerSpeed, sizeof(Vehicle.Axles[i].SteerSpeed));
//        CfgFile.write((char*)&Vehicle.Axles[i].Data.Slip, sizeof(Vehicle.Axles[i].Data.Slip));
//
//        for(unsigned int j = 0; j < 2; j++) {
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Mass, sizeof(Vehicle.Axles[i].Wheels[j].Mass));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].SteerMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].SteerMultiplier));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].DriveMultiplier, sizeof(Vehicle.Axles[i].Wheels[j].DriveMultiplier));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Damping, sizeof(Vehicle.Axles[i].Wheels[j].Damping));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Error, sizeof(Vehicle.Axles[i].Wheels[j].Error));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].StopERP, sizeof(Vehicle.Axles[i].Wheels[j].StopERP));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].StopCFM, sizeof(Vehicle.Axles[i].Wheels[j].StopCFM));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Bounce, sizeof(Vehicle.Axles[i].Wheels[j].Bounce));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].CFM, sizeof(Vehicle.Axles[i].Wheels[j].CFM));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].FMax, sizeof(Vehicle.Axles[i].Wheels[j].FMax));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].FudgeFactor, sizeof(Vehicle.Axles[i].Wheels[j].FudgeFactor));
//            CfgFile.write((char*)&Vehicle.Axles[i].Wheels[j].Data.Slip, sizeof(Vehicle.Axles[i].Wheels[j].Data.Slip));
//        }
//    }
//
//    CfgFile.write((char*)&Transmission, sizeof(Transmission));
//
//    CfgFile.close();
//
//    return true;
//}
//
//
//bool CVehicle::Create(glm::vec3 Centrum, glm::vec3 Rotation, int Boundaries)
//{
////    Centrum.y += 0.85f;
//    this->Center = Centrum;
//    this->Rotation = Rotation;
//    this->Boundaries = Boundaries;
//
//    dMass M;
//    dMatrix3 R;
//    glm::vec3 Min, Max, Dimensions;
//
//    Vehicle.Axles[1].Wheels[0].Position = GetRearAxleLeftTire();
//    Vehicle.Axles[1].Wheels[1].Position = GetRearAxleRightTire();
//    Vehicle.Axles[0].Wheels[0].Position = GetFrontAxleLeftTire();
//    Vehicle.Axles[0].Wheels[1].Position = GetFrontAxleRightTire();
//
//    Vehicle.Axles[0].Position = glm::vec3(0.0f, GetFrontAxleRightTire().y, GetFrontAxleRightTire().z);
//    Vehicle.Axles[1].Position = glm::vec3(0.0f, GetRearAxleRightTire().y, GetRearAxleRightTire().z);
//
//    VehicleSpace = dSimpleSpaceCreate(Space);
////    Vehicle.Body = dBodyCreate(World);
//
//    dVector3 *Vertices = new dVector3[GetBody().Header.NumVertices];
//    dTriIndex *Indices = new dTriIndex[GetBody().Faces.size() * 3];
//
//    for(int i = 0; i != GetBody().Header.NumVertices; i++) {
//        Vertices[i][0] = dReal(GetBody().Vertices[i].x);
//        Vertices[i][1] = dReal(GetBody().Vertices[i].y);
//        Vertices[i][2] = dReal(GetBody().Vertices[i].z);
//    }
//
//    int v = 0;
//
//    for(unsigned int i = 0; i != GetBody().Faces.size(); i++) {
//        Indices[v++] = GetBody().Faces[i].Vertices[2].Index;
//        Indices[v++] = GetBody().Faces[i].Vertices[1].Index;
//        Indices[v++] = GetBody().Faces[i].Vertices[0].Index;
//    }
//
//    dTriMeshDataID Data = dGeomTriMeshDataCreate();
//    dGeomTriMeshDataBuildSimple(Data, *Vertices, GetBody().Header.NumVertices, Indices, GetBody().Faces.size() * 3);
//    Vehicle.Geom = dCreateTriMesh(0, Data, 0, 0, 0);
//
//    Vehicle.Body = dBodyCreate(World);
//    dGeomSetBody(Vehicle.Geom, Vehicle.Body);
//
//    dSpaceAdd(VehicleSpace, Vehicle.Geom);
//
//    dGeomSetData(Vehicle.Geom, (void*)&Vehicle.Data);
//
////    Vehicle.COG = dBodyCreate(World);
////    dMassSetSphereTotal(&M, Vehicle.COGMass * Vehicle.MassScale, 1.0f);
////    dBodySetMass(Vehicle.COG, &M);
////    dBodySetPosition(Vehicle.COG, 0.0f, 0.0f, 0.0f);
//
////    for (int i = 0; i < 12; i++) {
////        CollisionPoints[i] = dCreateSphere(VehicleSpace, 0.001f);
////        dGeomSetBody(CollisionPoints[i], Vehicle.Body);
////        dGeomSetOffsetPosition(CollisionPoints[i], ScrapePoints[i].x, ScrapePoints[i].y, ScrapePoints[i].z);
////    }
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        Dimensions = GetAxle().GetBounds(Min, Max);
//
//        Vehicle.Axles[i].Body = dBodyCreate(World);
//        dBodySetFiniteRotationMode(Vehicle.Axles[i].Body, 1);
//
//        Vehicle.Axles[i].Geom = dCreateCylinder(VehicleSpace, Max.y, Dimensions.x);
//        dGeomSetBody(Vehicle.Axles[i].Geom, Vehicle.Axles[i].Body);
//
//        dMassSetCylinderTotal(&M, Vehicle.Axles[i].Mass * Vehicle.MassScale, 1, Max.y, Dimensions.x);
//        dBodySetMass(Vehicle.Axles[i].Body, &M);
//
//        dRFromEulerAngles(R, 0.0f, glm::radians(180.0f) * i, 0.0f);
//        dGeomSetRotation(Vehicle.Axles[i].Geom, R);
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            Dimensions = GetWheel(j).GetBounds(Min, Max);
//
//            Vehicle.Axles[i].Wheels[j].Body = dBodyCreate(World);
//            dBodySetFiniteRotationMode(Vehicle.Axles[i].Wheels[j].Body, 1);
//
//            Vehicle.Axles[i].Wheels[j].Geom = dCreateCylinder(VehicleSpace, Max.y, Dimensions.x);
//            dGeomSetBody(Vehicle.Axles[i].Wheels[j].Geom, Vehicle.Axles[i].Wheels[j].Body);
//
//            Dimensions = GetWheel(j).GetBounds(Min, Max);
//
//            dMassSetCylinderTotal(&M, Vehicle.Axles[i].Wheels[j].Mass * Vehicle.MassScale, 1, Max.y, Dimensions.x);
//            dBodySetMass(Vehicle.Axles[i].Wheels[j].Body, &M);
//
//            dRFromEulerAngles(R, 0.0f, glm::radians(90.0f), 0.0f);
//            dGeomSetRotation(Vehicle.Axles[i].Wheels[j].Geom, R);
//
////            dVector3 *Vertices = new dVector3[GetWheel(j).Header.NumVertices];
////            dTriIndex *Indices = new dTriIndex[GetWheel(j).Faces.size() * 3];
////
////            for ( int k = 0; k != GetWheel(j).Header.NumVertices; k++ ) {
////                Vertices[k][0] = dReal( GetWheel(j).Vertices[k].x);
////                Vertices[k][1] = dReal( GetWheel(j).Vertices[k].y);
////                Vertices[k][2] = dReal( GetWheel(j).Vertices[k].z);
////            }
////
////            int v = 0;
////            for (unsigned int k = 0; k != GetWheel(j).Faces.size(); k++) {
////                Indices[v++] = GetWheel(j).Faces[k].Vertices[2].Index;
////                Indices[v++] = GetWheel(j).Faces[k].Vertices[1].Index;
////                Indices[v++] = GetWheel(j).Faces[k].Vertices[0].Index;
////            }
////
////            Vehicle.Axles[i].Wheels[j].Body = dBodyCreate(World);
////            dBodySetFiniteRotationMode(Vehicle.Axles[i].Wheels[j].Body, 1);
////
////            dTriMeshDataID Data = dGeomTriMeshDataCreate();
////            dGeomTriMeshDataBuildSimple(Data, *Vertices, GetWheel(j).Header.NumVertices, Indices, GetWheel(j).Faces.size() * 3);
////            Vehicle.Axles[i].Wheels[j].Geom = dCreateTriMesh(0, Data, 0, 0, 0);
////
////            dRSetIdentity(R);
////            dRFromEulerAngles(R, 0.0f, glm::radians(90.0f), 0.0f);
////            dGeomSetRotation(Vehicle.Axles[i].Wheels[j].Geom, R);
////
////            dGeomSetBody(Vehicle.Axles[i].Wheels[j].Geom, Vehicle.Axles[i].Wheels[j].Body);
////            dGeomSetData(Vehicle.Axles[i].Wheels[j].Geom, (void*)&Vehicle.Axles[i].Wheels[j].Data);
////
////            dMatrix3 R;
////            dRSetIdentity(R);
////            dRFromEulerAngles(R, 0.0f, glm::radians(90.0f), 0.0f);
////            dGeomSetRotation(Vehicle.Axles[i].Wheels[j].Geom, R);
////
////            dSpaceAdd(VehicleSpace, Vehicle.Axles[i].Wheels[j].Geom);
//
//        }
//    }
//
////    SetPosition(glm::vec3(0));
//    SetOrientation(glm::vec3(0), glm::vec3(0));
//
////    Vehicle.COGJoint = dJointCreateFixed(World, 0);
////    dJointAttach(Vehicle.COGJoint, Vehicle.Body, Vehicle.COG);
////    dJointSetFixed(Vehicle.COGJoint);
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        Vehicle.Axles[i].DriveShaft = dJointCreateBall(World, 0);
//        dJointAttach(Vehicle.Axles[i].DriveShaft, Vehicle.Body, Vehicle.Axles[i].Body);
//        dJointSetBallAnchor(Vehicle.Axles[i].DriveShaft, GetDriveShaftPos().x, GetDriveShaftPos().y, GetDriveShaftPos().z);
//
////        Vehicle.Axles[i].DriveShaft = dJointCreateHinge(World, 0);
////        dJointAttach(Vehicle.Axles[i].DriveShaft, Vehicle.Body, Vehicle.Axles[i].Body);
////        dJointSetHingeAnchor(Vehicle.Axles[i].DriveShaft, GetDriveShaftPos().x, GetDriveShaftPos().y, GetDriveShaftPos().z);
////        dJointSetHingeAxis(Vehicle.Axles[i].DriveShaft, 0, 0, 1);
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            Vehicle.Axles[i].Hub[j] = dJointCreateBall(World, 0);
//            dJointAttach(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Body, Vehicle.Axles[i].Wheels[j].Body);
//            dJointSetBallAnchor(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z);
//
////            Vehicle.Axles[i].Hub[j] = dJointCreateHinge(World, 0);
////            dJointAttach(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Body, Vehicle.Axles[i].Wheels[j].Body);
////            dJointSetHingeAnchor(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z );
////            dJointSetHingeAxis(Vehicle.Axles[i].Hub[j], 1, 0, 0);
//
////            Vehicle.Axles[i].Hub[j] = dJointCreateUniversal(World, 0);
////            dJointAttach(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Body, Vehicle.Axles[i].Wheels[j].Body);
////            dJointSetUniversalAnchor(Vehicle.Axles[i].Hub[j], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z );
////            dJointSetUniversalAxis1(Vehicle.Axles[i].Hub[j], 0, 1, 0);
////            dJointSetUniversalAxis2(Vehicle.Axles[i].Hub[j], 1, 0, 0);
//
////            Vehicle.Axles[i].FourLink[j] = dJointCreateHinge(World, 0);
////            dJointAttach(Vehicle.Axles[i].FourLink[j], Vehicle.Body, Vehicle.Axles[i].Body);
////            dJointSetHingeAxis(Vehicle.Axles[i].FourLink[j], 0, 0, 1);
////
////            if (j == 0) {
////                dJointSetHingeAnchor(Vehicle.Axles[i].FourLink[j], AxleBarOffset.x, AxleBarOffset.y, AxleBarOffset.z);
////            } else {
////                dJointSetHingeAnchor(Vehicle.Axles[i].FourLink[j], -AxleBarOffset.x, AxleBarOffset.y, AxleBarOffset.z);
////            }
//
//            Vehicle.Axles[i].FourLink[j] = dJointCreateBall(World, 0);
//            dJointAttach(Vehicle.Axles[i].FourLink[j], Vehicle.Body, Vehicle.Axles[i].Body);
//
//            if(j == 0) {
//                dJointSetBallAnchor(Vehicle.Axles[i].FourLink[j], GetAxleBarOffset().x, GetAxleBarOffset().y, GetAxleBarOffset().z);
//            } else {
//                dJointSetBallAnchor(Vehicle.Axles[i].FourLink[j], -GetAxleBarOffset().x, GetAxleBarOffset().y, GetAxleBarOffset().z);
//            }
//
//            Vehicle.Axles[i].Wheels[j].ShockUpper[0] = dCreateSphere(VehicleSpace, 0.00001f);
//            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockUpper[0], Vehicle.Body);
//            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[0], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, Vehicle.Axles[i].Wheels[j].Position.z + 0.01f);
//
//            Vehicle.Axles[i].Wheels[j].ShockUpper[1] = dCreateSphere(VehicleSpace, 0.00001f);
//            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockUpper[1], Vehicle.Body);
//            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[1], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, Vehicle.Axles[i].Wheels[j].Position.z - 0.01f);
//
//            Vehicle.Axles[i].Wheels[j].ShockLower[0] = dCreateSphere(VehicleSpace, 0.00001f);
//            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockLower[0], Vehicle.Axles[i].Body);
//            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[0], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, +0.01f);
//
//            Vehicle.Axles[i].Wheels[j].ShockLower[1] = dCreateSphere(VehicleSpace, 0.00001f);
//            dGeomSetBody(Vehicle.Axles[i].Wheels[j].ShockLower[1], Vehicle.Axles[i].Body);
//            dGeomSetOffsetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[1], Vehicle.Axles[i].Wheels[j].Position.x / 2.0f, 0.0f, -0.01f);
//
//            Vehicle.Axles[i].Motor[j] = dJointCreateHinge2(World, 0);
//            dJointAttach(Vehicle.Axles[i].Motor[j], Vehicle.Body, Vehicle.Axles[i].Wheels[j].Body);
//
//            dJointSetHinge2Anchor(Vehicle.Axles[i].Motor[j], Vehicle.Axles[i].Wheels[j].Position.x, Vehicle.Axles[i].Wheels[j].Position.y, Vehicle.Axles[i].Wheels[j].Position.z);
//
//            dJointSetHinge2Axis1(Vehicle.Axles[i].Motor[j], 0, 1, 0);
//            dJointSetHinge2Axis2(Vehicle.Axles[i].Motor[j], 1, 0, 0);
//        }
//    }
//
//    SetPhysics();
//
////    SetPosition(Center);
//    SetOrientation(Center, Rotation);
//
//
//    return true;
//
//}
//
//void CVehicle::SetPosition(glm::vec3 Position)
//{
////    dMatrix3 R;
////    dRSetIdentity(R);
//
//    dBodySetLinearVel(Vehicle.Body, 0, 0, 0);
//    dBodySetAngularVel(Vehicle.Body, 0, 0, 0);
//
////    dBodySetRotation(Vehicle.Body, R);
//    dBodySetPosition(Vehicle.Body, Position.x, Position.y, Position.z);
//
//    for(int i = 0; i < 2; i++) {
//
//        dBodySetLinearVel(Vehicle.Axles[i].Body, 0, 0, 0);
//        dBodySetAngularVel(Vehicle.Axles[i].Body, 0, 0, 0);
//
////        dRSetIdentity(R);
////        dBodySetRotation(Vehicle.Axles[i].Body, R);
//        dBodySetPosition(Vehicle.Axles[i].Body, Vehicle.Axles[i].Position.x + Position.x, Vehicle.Axles[i].Position.y + Position.y, Vehicle.Axles[i].Position.z + Position.z);
//
//        for(int j = 0; j < 2; j++) {
//
//            dBodySetLinearVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);
//            dBodySetAngularVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);
//
////            dRSetIdentity(R);
////            dRFromEulerAngles(R, 0.0f, glm::radians(90.0f), 0.0f);
////            dBodySetRotation(Vehicle.Axles[i].Wheels[j].Body, R);
//            dBodySetPosition(Vehicle.Axles[i].Wheels[j].Body, Vehicle.Axles[i].Wheels[j].Position.x + Position.x, Vehicle.Axles[i].Wheels[j].Position.y + Position.y, Vehicle.Axles[i].Wheels[j].Position.z + Position.z);
//
//        }
//    }
//
//}
//
//void CVehicle::SetOrientation(glm::vec3 Position, glm::vec3 Rotation)
//{
////    MTM: theta, phi, psi
////    ODE: phi, theta, psi
//
//    float Model[16];
//
//    glLoadIdentity();
//    glPushMatrix();
//    glRotated(270.0f - glm::degrees(Rotation.x), 0, 1, 0);
//    glRotated(glm::degrees(-Rotation.z), 1, 0, 0);
//    glRotated(glm::degrees(Rotation.y), 0, 0, 1);
//    glGetFloatv(GL_MODELVIEW_MATRIX, Model);
//    glPopMatrix();
//
////    const dReal *Pos = dGeomGetPosition(Vehicle.Geom);
//
//    dMatrix3 R;
//    dRSetIdentity(R);
////    OGLtoODE(Model, Pos, R);
//
//    dBodySetLinearVel(Vehicle.Body, 0, 0, 0);
//    dBodySetAngularVel(Vehicle.Body, 0, 0, 0);
//
//    dBodySetRotation(Vehicle.Body, R);
//    dBodySetPosition(Vehicle.Body, Position.x, Position.y, Position.z);
//
////    dGeomSetPosition(Vehicle.Geom, Position.x, Position.y, Position.z);
////    dGeomSetRotation(Vehicle.Geom, R);
//
////    dBodySetLinearVel(Vehicle.COG, 0, 0, 0);
////    dBodySetAngularVel(Vehicle.COG, 0, 0, 0);
//
//    for(int i = 0; i < 2; i++) {
//
//        dMatrix3 R;
//        dRSetIdentity(R);
//
//        dBodySetLinearVel(Vehicle.Axles[i].Body, 0, 0, 0);
//        dBodySetAngularVel(Vehicle.Axles[i].Body, 0, 0, 0);
//
//        dBodySetRotation(Vehicle.Axles[i].Body, R);
//        dBodySetPosition(Vehicle.Axles[i].Body, Vehicle.Axles[i].Position.x + Position.x, Vehicle.Axles[i].Position.y + Position.y, Vehicle.Axles[i].Position.z + Position.z);
//
//        for(int j = 0; j < 2; j++) {
//
//            dBodySetLinearVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);
//            dBodySetAngularVel(Vehicle.Axles[i].Wheels[j].Body, 0, 0, 0);
//
//            dMatrix3 R;
//            dRSetIdentity(R);
//            dRFromEulerAngles(R, 0.0f, glm::radians(90.0f), 0);
//
//            dBodySetPosition(Vehicle.Axles[i].Wheels[j].Body, Vehicle.Axles[i].Wheels[j].Position.x + Position.x, Vehicle.Axles[i].Wheels[j].Position.y + Position.y, Vehicle.Axles[i].Wheels[j].Position.z + Position.z);
//            dBodySetRotation(Vehicle.Axles[i].Wheels[j].Body, R);
//
//        }
//    }
//
//}
//
//void CVehicle::Reset(void)
//{
//    SetOrientation(Center, Rotation);
//
//}
//
//void CVehicle::Flip(void)
//{
//
//    glm::vec3 Position = GetPosition();
//    Position.y += 0.85f;
//
//    glm::vec3 Rotate = Rotation;
//    Rotate.x = glm::radians(Config->Camera.Angle);
//
//    SetOrientation(Position, Rotate);
//
//}
//
//bool CVehicle::Update(dReal Delta)
//{
//
//    Velocity = GetVelocity();
//    Transmission.Rpms = int(Velocity * Transmission.MaxRpms / Transmission.GearNext[Transmission.Gear][2]);
//    Transmission.Speed = int(Velocity * 3600.0f / 1000.0f);
//
//    if(Transmission.Gear > 0) {
//        AutomaticGear();
//        CheckGear(Delta);
//    } else {
//        Transmission.Speed = 0.0f;
//        Transmission.Rpms = 0.0f;
//    }
//
//    if(((glfwGetKey(GLWindow, Config->Input.DownShift) == GLFW_RELEASE) || Input->GetButtonState(Config->Joypad.DownShift)) && (Transmission.Gear > 0)) {
//        Transmission.Gear--;
//    }
//
//    if(((glfwGetKey(GLWindow, Config->Input.UpShift) == GLFW_RELEASE) || Input->GetButtonState(Config->Joypad.UpShift)) && (Transmission.Gear < Transmission.GearCount)) {
//        Transmission.Gear++;
//    }
//
////    if ( Transmission.Speed < 0 ) {
////        Transmission.Speed = 0.0f;
////    }
//
//    if (Input->GetButtonState(Config->Joypad.Reset)) {
//        Reset();
//    }
//
//    if (Input->GetButtonState(Config->Joypad.Flip)) {
//        Flip();
//    }
//
//    if(glfwGetKey(GLWindow, Config->Input.Gas) == GLFW_PRESS || Input->GetButtonState(Config->Joypad.Gas)) {
//
//        Vehicle.Speed = Transmission.MaxSpeed;
//        Vehicle.WheelForce = Transmission.Acceleration;
//
//    } else if(glfwGetKey(GLWindow, Config->Input.Brake) == GLFW_PRESS || Input->GetButtonState(Config->Joypad.Brake)) {
//
//        Vehicle.Speed = Transmission.ReverseSpeed;
//        Vehicle.WheelForce = Transmission.Acceleration;
//
//    } else {
//
//        Vehicle.Speed = 0;
//        Vehicle.WheelForce = 0;
//    }
//
//    if(glfwGetKey(GLWindow, Config->Input.FrontSteerRight) || (Input->GetAxisState(abs(Config->Joypad.FrontSteerRight)) > 0.25f)) {
//        Vehicle.Axles[0].SteerAngle += Vehicle.Axles[0].SteerSpeed * Delta;
//    } else if(glfwGetKey(GLWindow, Config->Input.FrontSteerLeft) || (Input->GetAxisState(abs(Config->Joypad.FrontSteerLeft)) < -0.25f)) {
//        Vehicle.Axles[0].SteerAngle -= Vehicle.Axles[0].SteerSpeed * Delta;
//    } else {
//        if(Vehicle.Axles[0].SteerAngle < 0) {
//            Vehicle.Axles[0].SteerAngle += Vehicle.Axles[0].SteerSpeed * Delta;
//        } else if(Vehicle.Axles[0].SteerAngle > 0) {
//            Vehicle.Axles[0].SteerAngle -= Vehicle.Axles[0].SteerSpeed * Delta;
//        }
//    }
//
//
//    Vehicle.Axles[0].SteerAngle = glm::min(Vehicle.Axles[0].SteerAngle, Vehicle.Axles[0].MaxSteer);
//    Vehicle.Axles[0].SteerAngle = glm::max(Vehicle.Axles[0].SteerAngle, Vehicle.Axles[0].MinSteer);
//
//    if(glfwGetKey(GLWindow, Config->Input.RearSteerRight)) {
//        Vehicle.Axles[1].SteerAngle += Vehicle.Axles[1].SteerSpeed * Delta;
//    } else if(glfwGetKey(GLWindow, Config->Input.RearSteerLeft)) {
//        Vehicle.Axles[1].SteerAngle -= Vehicle.Axles[1].SteerSpeed * Delta;
//    } else {
//        if(Vehicle.Axles[1].SteerAngle < 0) {
//            Vehicle.Axles[1].SteerAngle += Vehicle.Axles[1].SteerSpeed * Delta;
//        } else if(Vehicle.Axles[1].SteerAngle > 0) {
//            Vehicle.Axles[1].SteerAngle -= Vehicle.Axles[1].SteerSpeed * Delta;
//        }
//    }
//
//    Vehicle.Axles[1].SteerAngle = glm::min(Vehicle.Axles[1].SteerAngle, Vehicle.Axles[1].MaxSteer);
//    Vehicle.Axles[1].SteerAngle = glm::max(Vehicle.Axles[1].SteerAngle, Vehicle.Axles[1].MinSteer);
//
//    dVector3 Axis;
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamLoStop, Vehicle.Axles[i].SteerAngle * Vehicle.Axles[i].Wheels[j].SteerMultiplier);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamHiStop, Vehicle.Axles[i].SteerAngle * Vehicle.Axles[i].Wheels[j].SteerMultiplier);
//
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamVel2, Vehicle.Speed * Vehicle.Axles[i].Wheels[j].DriveMultiplier);
//            dJointSetHinge2Param(Vehicle.Axles[i].Motor[j], dParamFMax2, Vehicle.WheelForce * Vehicle.Axles[i].Wheels[j].DriveMultiplier);
//
//            dJointGetHinge2Axis2(Vehicle.Axles[i].Motor[j], Axis);
//            dBodySetFiniteRotationAxis(Vehicle.Axles[i].Wheels[j].Body, Axis[0], Axis[1], Axis[2]);
//
//        }
//
//    }
//
////    Transmission.Rpms += 1000.0f;
//
////    glm::vec3 VehiclePos = GetPosition();
////
////    if (VehiclePos.x < 0.0f) {
////        VehiclePos.x += Boundaries;
////        SetPosition(VehiclePos);
////    }
////
////    if (VehiclePos.z < 0.0f) {
////        VehiclePos.z += Boundaries;
////        SetPosition(VehiclePos);
////    }
////
////    if (VehiclePos.x > Boundaries) {
////        VehiclePos.x -= Boundaries;
////        SetPosition(VehiclePos);
////    }
////
////    if (VehiclePos.z > Boundaries) {
////        VehiclePos.z -= Boundaries;
////
////    }
//
//    return true;
//
//}
//
//void CVehicle::Render(void)
//{
//    double Matrix[16];
//
//    const dReal *Position = dBodyGetPosition(Vehicle.Body);
//    const dReal *Rotation = dBodyGetRotation(Vehicle.Body);
//
//    ODEtoOGL(Matrix, Position, Rotation);
//
//    glPushMatrix();
//
//    glMultMatrixd(Matrix);
//    glCallList(GetBody().GetModel());
//
//    glPopMatrix();
//
//    dVector3 DriveShaftPosition;
//    dVector3 HubPosition;
//    dVector3 FourLinkPosition;
////    dVector3 ShockPosition;
//
//    for(unsigned int i = 0; i < 2; i++) {
//
//        const dReal *AxlePosition = dBodyGetPosition(Vehicle.Axles[i].Body);
//        dJointGetBallAnchor(Vehicle.Axles[i].DriveShaft, DriveShaftPosition);
////        dJointGetHingeAnchor(Vehicle.Axles[i].DriveShaft, DriveShaftPosition);
//
//        GetDriveShaftTexture().Bind(0);
//        Cylinder(glm::vec3(AxlePosition[0], AxlePosition[1], AxlePosition[2]), glm::vec3(DriveShaftPosition[0], DriveShaftPosition[1], DriveShaftPosition[2]), 0.006125f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);
//
//        const dReal *Position = dBodyGetPosition(Vehicle.Axles[i].Body);
//        const dReal *Rotation = dBodyGetRotation(Vehicle.Axles[i].Body);
//
//        ODEtoOGL(Matrix, Position, Rotation);
//
//        glPushMatrix();
//
//        glMultMatrixd(Matrix);
//
//        glCallList(GetAxle().GetModel());
//
//        glPopMatrix();
//
//        for(unsigned int j = 0; j < 2; j++) {
//
//            dJointGetBallAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
////            dJointGetHingeAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
////            dJointGetUniversalAnchor(Vehicle.Axles[i].Hub[j], HubPosition);
//
//            dJointGetBallAnchor(Vehicle.Axles[i].FourLink[j], FourLinkPosition);
////            dJointGetHingeAnchor(Vehicle.Axles[i].FourLink[j], FourLinkPosition);
//
//            GetBarTexture().Bind(0);
//            Cylinder(glm::vec3(FourLinkPosition[0], FourLinkPosition[1], FourLinkPosition[2]), glm::vec3(HubPosition[0], HubPosition[1], HubPosition[2]), 0.006125f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);
//
//            const dReal *ShockPositionUpper1 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[0]);
//            const dReal *ShockPositionUpper2 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockUpper[1]);
//            const dReal *ShockPositionLower1 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[0]);
//            const dReal *ShockPositionLower2 = dGeomGetPosition(Vehicle.Axles[i].Wheels[j].ShockLower[1]);
//
//            GetShockTexture().Bind(0);
//            Cylinder(glm::vec3(ShockPositionUpper1[0], ShockPositionUpper1[1], ShockPositionUpper1[2]), glm::vec3(ShockPositionLower1[0], ShockPositionLower1[1], ShockPositionLower1[2]), 0.0030625f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);
//            Cylinder(glm::vec3(ShockPositionUpper2[0], ShockPositionUpper2[1], ShockPositionUpper2[2]), glm::vec3(ShockPositionLower2[0], ShockPositionLower2[1], ShockPositionLower2[2]), 0.0030625f, 6, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), Config->Render.Lighted);
//
//            const dReal *Position = dBodyGetPosition(Vehicle.Axles[i].Wheels[j].Body);
//            const dReal *Rotation = dBodyGetRotation(Vehicle.Axles[i].Wheels[j].Body);
//
//            ODEtoOGL(Matrix, Position, Rotation);
//
//            glPushMatrix();
//
//            glMultMatrixd(Matrix);
//
//            glRotatef(90.0f, 0, 1, 0);
//            glCallList(GetWheel(j).GetModel());
//
//            glPopMatrix();
//
//        }
//
//    }
//
////    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
////    glPointSize(14);
////    glBegin(GL_POINTS);
////
////    for ( int i = 0; i < 12; i++ ) {
////
////        const dReal *Position = dGeomGetPosition(CollisionPoints[i]);
////        const dReal *Rotation = dGeomGetRotation(CollisionPoints[i]);
////
////        ODEtoOGL(Matrix, Position, Rotation);
////
////        glPushMatrix();
////
////            glMultMatrixf(Matrix);
////
////            glVertex3f(Position[0], Position[1], Position[2]);
////
////
////        glPopMatrix();
////
////    }
////    glEnd();
//
//}
//
//float CVehicle::GetVelocity(void)
//{
//    dReal const *CV = dBodyGetLinearVel(Vehicle.Body);
//    float V2 = (CV[0] * CV[0] + /*CV[1] * CV[1] +*/ CV[2] * CV[2]);
//    float V = sqrtf(V2) / 1000 * 3600;
//
//    return  V;
//}
//
