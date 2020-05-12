
#ifndef VEHICLES_HPP
#define VEHICLES_HPP

#include "../Common.hpp"

#include "Pods.hpp"
#include "Models.hpp"
#include "Textures.hpp"

namespace Game
{

class CTruck
{

private:

    bool MTM2Truck;

    std::string Name;

    std::string BodyModel;
    std::string TireModel;
    std::string AxleModel;

    std::string ShockFilename;
    std::string BarFilename;

    glm::vec3 AxleBarOffset;
    glm::vec3 DriveShaftPos;

    glm::vec3 FrontAxleLeftTire;
    glm::vec3 FrontAxleRightTire;
    glm::vec3 RearAxleLeftTire;
    glm::vec3 RearAxleRightTire;

    glm::vec3 ScrapePoints[12];

    CModel Body;
    CModel Axles;
    CModel Wheels[2];

    CTexture ShockTexture;
    CTexture BarTexture;
    CTexture DriveShaftTexture;

public:

    CTruck(void);
    ~CTruck(void);

    bool Load(CPodPool &PodPool, std::string Filename);

    CModel &GetBody(void)
    {
        return Body;
    }
    CModel &GetAxle(void)
    {
        return Axles;
    }
    CModel &GetWheel(int Index)
    {
        return Wheels[Index];
    }

    CTexture &GetShockTexture(void)
    {
        return ShockTexture;
    }
    CTexture &GetBarTexture(void)
    {
        return BarTexture;
    }
    CTexture &GetDriveShaftTexture(void)
    {
        return DriveShaftTexture;
    }
    glm::vec3 &GetScrapePoint(int Index)
    {
        return ScrapePoints[Index];
    }
    glm::vec3 &GetAxleBarOffset(void)
    {
        return AxleBarOffset;
    }
    glm::vec3 &GetDriveShaftPos(void)
    {
        return DriveShaftPos;
    }

    glm::vec3 &GetFrontAxleLeftTire(void)
    {
        return FrontAxleLeftTire;
    }
    glm::vec3 &GetFrontAxleRightTire(void)
    {
        return FrontAxleRightTire;
    }
    glm::vec3 &GetRearAxleLeftTire(void)
    {
        return RearAxleLeftTire;
    }
    glm::vec3 &GetRearAxleRightTire(void)
    {
        return RearAxleRightTire;
    }


};

class CTruckPool
{

private:

    std::vector<CTruck> Trucks;

public:

    CTruckPool(void);
    ~CTruckPool(void);

    bool Create(CPodPool &PodPool, std::vector<std::string> &Filenames);

};

}

#endif
