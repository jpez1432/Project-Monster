
#include "Map.hpp"

using namespace Game;

CMap::CMap(void)
{


}

CMap::~CMap(void)
{

    if (TexturePool) {
        delete TexturePool;
        TexturePool = NULL;
        Logger->Text("Destroyed Texture Pool Object");
    }

    if (Terrain) {
        delete Terrain;
        Terrain = NULL;
        Logger->Text("Destroyed Terrain Object");
    }

    if (Boxes) {
        delete Boxes;
        Boxes = NULL;
        Logger->Text("Destroyed Ground Box Object");
    }

    if (ModelPool) {
        delete ModelPool;
        ModelPool = NULL;
        Logger->Text("Destroyed Model Pool Object");
    }

}

bool CMap::Load(CPodPool &PodPool, std::string Filename)
{

    std::string Line;
    int Size, Offset;

    Filename.insert(0, "World\\");
    PodPool.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::in);

    if (StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    std::getline(StreamIn, Level);
    Name = FindMarker(StreamIn, "!Race Track Name");
    Locale = FindMarker(StreamIn, "Race Track Locale");
    Logo = FindMarker(StreamIn, "Track Logo .BMP file");
    Description = FindMarker(StreamIn, "Track Description .TXT file");
    Type = FindMarker(StreamIn, "Track Race Type");

    Line = FindMarker(StreamIn, "*** Vehicles ***");
    Vehicles.resize(atoi(Line.c_str()));
    std::getline(StreamIn, Line);

    for (unsigned int i = 0; i < Vehicles.size(); i++) {

        Vehicles[i].Filename = FindMarker(StreamIn, "truckFile");
        ParseVector(FindMarker(StreamIn, "ipos"), Vehicles[i].Position);
        ParseVector(FindMarker(StreamIn, "theta,phi,psi"), Vehicles[i].Rotation);

    }

    Line = FindMarker(StreamIn, "*** Boxes ***");
    Models.resize(atoi(Line.c_str()));
    std::getline(StreamIn, Line);

    for (unsigned int i = 0; i < Models.size(); i++) {

        ParseVector(FindMarker(StreamIn, "ipos"), Models[i].Position);
        ParseVector(FindMarker(StreamIn, "theta,phi,psi"), Models[i].Rotation);

        Line = FindMarker(StreamIn, "model", "length,width,height");

        if (isdigit(Line[0])) {
            ParseVector(Line, Models[i].Dimensions);

        } else {
            Models[i].Filename = Line;
        }

        Models[i].Mass = atof(FindMarker(StreamIn, "mass").c_str());
        Models[i].TypeFlags  = FindMarker(StreamIn, "!type,flags");
        Models[i].Priority = atoi(FindMarker(StreamIn, "priority").c_str());

    }

    Backdrop = FindMarker(StreamIn, "backdropModelName");

    StreamIn.close();

    Filename = "Levels\\" + Level;
    PodPool.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in);

    if (StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    std::getline(StreamIn, Line);
    std::getline(StreamIn, TextFile);
    std::getline(StreamIn, HeightMap);
    std::getline(StreamIn, TextureInfo);
    std::getline(StreamIn, Palette);
    std::getline(StreamIn, TextureList);

    for (int i = 0; i < 10; i++) {
        std::getline(StreamIn, Line);
    }

    std::getline(StreamIn, LightMap);

    std::getline(StreamIn, Line);
    ParseVector(Line, SunPosition);

    SunPosition /= 64.0f;
    SunPosition.x *= -1.0f;

    float Temp = SunPosition.z;
    SunPosition.z = SunPosition.x;
    SunPosition.x = Temp;

    for (int i = 0; i < 5; i++) {

        if (!StreamIn.eof()) {

            std::getline(StreamIn, Line);

            if (Line == "!waterHeight") {

                std::getline(StreamIn, Line);
                WaterHeight = atof(Line.c_str()) / 64.0f;

            } else {

                WaterHeight = -1.0f;

            }
        }
    }

    StreamIn.close();

    Filename = "Data\\" + TextureList;
    PodPool.FindFile(Filename, Size, Offset);

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in);

    if (StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    std::getline(StreamIn, Line);
    std::vector<std::string> Filenames(atoi(Line.c_str()));

    for (unsigned int i = 0; i < Filenames.size(); i++) {
        std::getline(StreamIn, Filenames[i]);
    }

    StreamIn.close();

    TexturePool = new CTexturePool();

    if (TexturePool->Create(PodPool, Filenames)) {
        Logger->Text("Loaded Map Textures");
    } else {
        Logger->Error("Error Loading Map Textures");
    }

    Terrain = new CTerrain();

    if (Terrain->Load(PodPool, HeightMap, TextureInfo)) {
        Logger->Text("Loaded Terrain Heightmap & Texture Info");
    } else {
        Logger->Error("Error Loading Terrain Heightmap & Texture Info");
    }

    Boxes = new CBoxes();

    if (Boxes->Load(PodPool, Level, Level)) {
        Logger->Text("Loaded Ground Boxes & Texture Info");
    } else {
        Logger->Error("Error Loading Ground Boxes & Texture Info");
    }

    Filenames.resize(0);

    for (unsigned int i = 0; i < Models.size(); i++) {
        if (Models[i].Filename != "") {
            Filenames.push_back(Models[i].Filename);
        }
    }

    std::sort(Filenames.begin(), Filenames.end());
    Filenames.erase(std::unique(Filenames.begin(), Filenames.end()), Filenames.end());

    ModelPool = new CModelPool();

    if (ModelPool->Create(PodPool, Filenames)) {
        Logger->Text("Loaded Map Models");
    } else {
        Logger->Error("Error Loading Map Models");
    }

    for (unsigned int j = 0; j < Models.size(); j++) {
        for (unsigned int i = 0; i < Filenames.size(); i++) {
            if ((strcasecmp(ModelPool->Filename(i).c_str(), Models[j].Filename.c_str()) == 0)) {
                Models[j].GLModel = ModelPool->GetModel(i);
                Models[j].Bounds = ModelPool->GetBoundingSphere(i);
            }
        }
    }

    WaterTexture.LoadRaw(PodPool, "Rippl100.raw");

    if (Background.Load(PodPool, Backdrop)) {
        Logger->Text("Loaded Map Background Model - " + Backdrop);
    } else {
        Logger->Error("Error Loading Map Background Model - " + Backdrop);
    }

    return true;

}

std::string CMap::FindMarker(std::ifstream &Stream, std::string Marker1, std::string Marker2)
{

    std::string Line;

    do {
        std::getline(Stream, Line);
    } while ((Line != Marker1) && (Line != Marker2));

    if (Stream.eof()) {
        return Line;
    }

    if (Line == Marker1) {
        std::getline(Stream, Line);
    }

    if (Line == Marker2) {
        std::getline(Stream, Line);
    }

    return Line;

}

