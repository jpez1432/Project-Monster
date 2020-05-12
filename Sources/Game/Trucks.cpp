
#include "Trucks.hpp"

using namespace Game;

CTruck::CTruck(void)
{


}

CTruck::~CTruck(void)
{


}

bool CTruck::Load(CPodPool &PodPool, std::string Filename)
{

    int Size, Offset;
    std::string Str;

    Filename.insert(0, "Truck\\");
    PodPool.FindFile(Filename, Size, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    if(Offset != 0) {
        StreamIn.seekg(Offset, std::ios::beg);
    }

    std::getline(StreamIn, Str);

    if((StrCompare(Str.c_str(), "MTM2 truckName")) || StrCompare(Str.c_str(), "MTM2.1 truckName")) {

        MTM2Truck = true;

        std::getline(StreamIn, Name);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, BodyModel);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, TireModel);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, AxleModel);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, ShockFilename);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, BarFilename);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);

        ParseVector(Str, AxleBarOffset);
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);

        ParseVector(Str, DriveShaftPos);
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);

        FrontAxleRightTire.x = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleLeftTire.x = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleRightTire.x = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleLeftTire.x = atof(Str.c_str());

        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleRightTire.y = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleLeftTire.y = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleRightTire.y = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleLeftTire.y = atof(Str.c_str());

        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleRightTire.z = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleLeftTire.z = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleRightTire.z = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleLeftTire.z = atof(Str.c_str());

        for(unsigned int i = 0; i < 12; i++)  {

            std::getline(StreamIn, Str);
            std::getline(StreamIn, Str);
            ParseVector(Str, ScrapePoints[i]);

            float Swap = ScrapePoints[i].z;
            ScrapePoints[i].z = -ScrapePoints[i].x;
            ScrapePoints[i].x = -Swap;
            ScrapePoints[i] /= 32.0f;
        }

        float Swap = DriveShaftPos.z;
        DriveShaftPos.z = -DriveShaftPos.x;
        DriveShaftPos.x = -Swap;
        DriveShaftPos /= 32.0f;

        Swap = AxleBarOffset.z;
        AxleBarOffset.z = -AxleBarOffset.x;
        AxleBarOffset.x = -Swap;
        AxleBarOffset /= 32.0f;

        ShockTexture.LoadRaw(PodPool, ShockFilename);
        BarTexture.LoadRaw(PodPool, BarFilename);
        DriveShaftTexture.LoadRaw(PodPool, "BARDRV.raw");

    } else {

        MTM2Truck = false;

        AxleModel = "Axle.bin";

        std::getline(StreamIn, Name);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, BodyModel);

        std::getline(StreamIn, Str);
        std::getline(StreamIn, TireModel);

        std::getline(StreamIn, Str);

        std::getline(StreamIn, Str);
        FrontAxleRightTire.x = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleLeftTire.x = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleRightTire.x = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleLeftTire.x = atof(Str.c_str());

        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleRightTire.y = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleLeftTire.y = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleRightTire.y = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleLeftTire.y = atof(Str.c_str());

        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleRightTire.z = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        FrontAxleLeftTire.z = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleRightTire.z = atof(Str.c_str());
        std::getline(StreamIn, Str);
        std::getline(StreamIn, Str);
        RearAxleLeftTire.z = atof(Str.c_str());

        for(unsigned int i = 0; i < 12; i++)  {

            std::getline(StreamIn, Str);
            std::getline(StreamIn, Str);
            ParseVector(Str, ScrapePoints[i]);

            float Swap = ScrapePoints[i].z;
            ScrapePoints[i].z = ScrapePoints[i].x;
            ScrapePoints[i].x = -Swap;
            ScrapePoints[i] /= 32.0f;
        }

//        DriveShaftPos = glm::vec3(0);
//        AxleBarOffset = glm::vec3(0);

        DriveShaftPos = glm::vec3(0.000000f, -2.456250f, -0.113281f);
        AxleBarOffset = glm::vec3(1.281250f, -3.156250f, -0.113281f);

        float Swap = DriveShaftPos.z;
        DriveShaftPos.z = DriveShaftPos.x;
        DriveShaftPos.x = -Swap;
        DriveShaftPos /= 32.0f;

        Swap = AxleBarOffset.z;
        AxleBarOffset.z = AxleBarOffset.x;
        AxleBarOffset.x = -Swap;
        AxleBarOffset /= 32.0f;

        ShockTexture.LoadRaw(PodPool, "shock2.raw");
        BarTexture.LoadRaw(PodPool, "axlebar2.raw");
        DriveShaftTexture.LoadRaw(PodPool, "BARDRV.raw");

    }

    StreamIn.close();

    FrontAxleRightTire /= glm::vec3(32.0f, 32.0f, -32.0f);
    FrontAxleLeftTire /= glm::vec3(32.0f, 32.0f, -32.0f);
    RearAxleRightTire /= glm::vec3(32.0f, 32.0f, -32.0f);
    RearAxleLeftTire /= glm::vec3(32.0f, 32.0f, -32.0f);

    if(BodyModel.find(".") == std::string::npos) {
        BodyModel += ".bin";
    }

    if(Body.Load(PodPool, BodyModel)) {
        Logger->Text("Loaded Vehicle Body Model - " + BodyModel);
    } else {
        Logger->Error("Error Loading Vehicle Body Model - " + BodyModel);
    }

    std::string LeftTire;
    std::string RightTire;

    if(TireModel.find(".") == std::string::npos) {
        LeftTire += TireModel + "16L.bin";
        RightTire += TireModel + "16R.bin";
    } else {
        LeftTire = TireModel;
        RightTire = TireModel;
    }

    if(Wheels[0].Load(PodPool, LeftTire)) {
        Logger->Text("Loaded Vehicle Left Tire Model - " + LeftTire);
    } else {
        Logger->Error("Error Loading Vehicle Left Tire Model - " + LeftTire);
    }

    if(Wheels[1].Load(PodPool, RightTire)) {
        Logger->Text("Loaded Vehicle Right Tire Model - " + RightTire);
    } else {
        Logger->Error("Error Loading Vehicle Right Tire Model - " + RightTire);
    }

    if(Axles.Load(PodPool, AxleModel)) {
        Logger->Text("Loaded Vehicle Axle Model - " + AxleModel);
    } else {
        Logger->Error("Error Loading Vehicle Axle Model - " + AxleModel);
    }

    return true;

}

CTruckPool::CTruckPool(void)
{


}

CTruckPool::~CTruckPool(void)
{

    Trucks.clear();

}

bool CTruckPool::Create(CPodPool &PodPool, std::vector<std::string> &Filenames)
{

    Trucks.resize(Filenames.size());

    for(unsigned int i = 0; i < Trucks.size(); i++) {
        Trucks[i].Load(PodPool, Filenames[i].c_str());
    }

    return true;

}

