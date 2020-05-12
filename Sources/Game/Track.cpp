
#include "Track.hpp"

using namespace Game;

CTrack::CTrack(void) : TexturePool(NULL), ModelPool(NULL), Terrain(NULL), Boxes(NULL)
{

}

CTrack::~CTrack(void)
{
    Destroy();
}

bool CTrack::Load(CPodPool &PodPool, std::string Filename)
{

    std::string Line;
    int Size, Offset;

    Filename.insert(0, "World\\");
    PodPool.FindFile(Filename, Size, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
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

    for(unsigned int i = 0; i < Vehicles.size(); i++) {

        Vehicles[i].Filename = FindMarker(StreamIn, "truckFile");
        ParseVector(FindMarker(StreamIn, "ipos"), Vehicles[i].Position);
        ParseVector(FindMarker(StreamIn, "theta,phi,psi"), Vehicles[i].Rotation);

        Vehicles[i].Position /= 32.0f;
    }

    Line = FindMarker(StreamIn, "*** Boxes ***");
    Models.resize(atoi(Line.c_str()));
    std::getline(StreamIn, Line);

    for(unsigned int i = 0; i < Models.size(); i++) {

        ParseVector(FindMarker(StreamIn, "ipos"), Models[i].Position);
        ParseVector(FindMarker(StreamIn, "theta,phi,psi"), Models[i].Rotation);

        if (Models[i].Position.x < 0) { Models[i].Position.x = abs((256 * 32) + Models[i].Position.x); }
    //    if (Vector.y < 0) { Vector.y = abs((256 * 32) + Vector.y); }
        if (Models[i].Position.z < 0) { Models[i].Position.z = abs((256 * 32) + Models[i].Position.z); }

        Models[i].Position /= 32.0f;

        Line = FindMarker(StreamIn, "model", "length,width,height");

        if (StrCompare(Line.substr(Line.size() - 4, Line.size()), ".BIN")) {
            Models[i].Filename = Line;
        } else {
            Models[i].Filename = "Box";
            ParseVector(Line, Models[i].Dimensions);
            Models[i].Dimensions /= 32.0f;
        }

//        if(isdigit(Line[0])) {
//            ParseVector(Line, Models[i].Dimensions);
//            Models[i].Dimensions /= 32.0f;
//        } else {
//            Models[i].Filename = Line;
//        }

        Models[i].Mass = atof(FindMarker(StreamIn, "mass").c_str());
        Models[i].TypeFlags  = FindMarker(StreamIn, "!type,flags");
        Models[i].Priority = atoi(FindMarker(StreamIn, "priority").c_str());

    }

    Line = FindMarker(StreamIn, "*** Stadium ***");

    if(StrCompare(Line.c_str(), "!stadiumFlag,x,z,sx,sz,stadiumModelName")) {
        MTM2Track = true;
    }

    Backdrop = FindMarker(StreamIn, "backdropType,backdropCount");
    BackdropCount = atoi(Backdrop.substr(Backdrop.find_last_of(",") + 1, Backdrop.length()).c_str());
    std::getline(StreamIn, Backdrop);

    Backdrops.resize(BackdropCount);

    for(int i = 0; i < BackdropCount; i++) {
        std::getline(StreamIn, Backdrop);

        if(Backdrops[i].Load(PodPool, Backdrop)) {
            Logger->Text("Loaded Track Background Model - " + Backdrop);
        } else {
            Logger->Error("Error Track Map Background Model - " + Backdrop);
        }
    }

    StreamIn.close();

    Filename = "Levels\\" + Level;
    PodPool.FindFile(Filename, Size, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    std::getline(StreamIn, Line);
    std::getline(StreamIn, TextFile);
    std::getline(StreamIn, HeightMap);
    std::getline(StreamIn, TextureInfo);
    std::getline(StreamIn, Palette);
    std::getline(StreamIn, TextureList);

    for(int i = 0; i < 10; i++) {
        std::getline(StreamIn, Line);
    }

    std::getline(StreamIn, LightMap);

    std::getline(StreamIn, Line);
    ParseVector(Line, SunPosition);

    SunPosition /= 64.0f;

    Config->Render.Water = false;
    WaterHeight = -1.0f;

    for(int i = 0; i < 6; i++) {
        std::getline(StreamIn, Line);
    }

    WaterHeight = atof(Line.c_str()) / 64.0f;

    if(WaterHeight != -1.0f) {
        Config->Render.Water = true;
    }

    StreamIn.close();

    Filename = "Data\\" + TextureList;
    PodPool.FindFile(Filename, Size, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    Filename.insert(0, RootDir + ResourceDir);
    StreamIn.open(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, StreamIn.beg);

    std::getline(StreamIn, Line);
    std::vector<std::string> Filenames(atoi(Line.c_str()));

    for(unsigned int i = 0; i < Filenames.size(); i++) {
        std::getline(StreamIn, Filenames[i]);
    }

    StreamIn.close();

    TexturePool = new CTexturePool();
    if(TexturePool->Create(PodPool, Filenames)) {
        Logger->Text("Loaded Map Textures");
    } else {
        Logger->Error("Error Loading Map Textures");
    }

    Terrain = new CTerrain();
    if(Terrain->Load(PodPool, HeightMap, TextureInfo)) {
        Logger->Text("Loaded Terrain Heightmap & Texture Info");
    } else {
        Logger->Error("Error Loading Terrain Heightmap & Texture Info");
    }

    Boxes = new CBoxes();
    if(Boxes->Load(PodPool, Level, Level)) {
        Logger->Text("Loaded Ground Boxes & Texture Info");
    } else {
        Logger->Error("Error Loading Ground Boxes & Texture Info");
    }

    Filenames.resize(0);
    for(unsigned int i = 0; i < Models.size(); i++) {
        if(Models[i].Filename != "Box") {
            Filenames.push_back(Models[i].Filename);
        }
    }

    std::sort(Filenames.begin(), Filenames.end());
    Filenames.erase(std::unique(Filenames.begin(), Filenames.end()), Filenames.end());

    ModelPool = new CModelPool();
    if(ModelPool->Create(PodPool, Filenames)) {
        Logger->Text("Loaded Map Models");
    } else {
        Logger->Error("Error Loading Map Models");
    }

    for(unsigned int j = 0; j < Models.size(); j++) {
        for(unsigned int i = 0; i < Filenames.size(); i++) {
            if((StrCompare(ModelPool->Filename(i).c_str(), Models[j].Filename.c_str()))) {
                Models[j].Index = i;
            }
        }
    }

    WaterTexture.LoadRaw(PodPool, "Rippl100.raw");

    return true;

}

void CTrack::Destroy(void)
{
    if(TexturePool) {
        delete TexturePool;
        TexturePool = NULL;
        Logger->Text("Destroyed Texture Pool Object");
    }

    if(ModelPool) {
        delete ModelPool;
        ModelPool = NULL;
        Logger->Text("Destroyed Model Pool Object");
    }

    if(Terrain) {
        delete Terrain;
        Terrain = NULL;
        Logger->Text("Destroyed Terrain Object");
    }

    if(Boxes) {
        delete Boxes;
        Boxes = NULL;
        Logger->Text("Destroyed Ground Box Object");
    }
}

std::string CTrack::FindMarker(std::ifstream &Stream, std::string Marker1, std::string Marker2)
{

    std::string Line;

    do {
        std::getline(Stream, Line);
    } while((Line != Marker1) && (Line != Marker2) && !Stream.eof());

    std::getline(Stream, Line);

//    if(Line == Marker1) {
//        std::getline(Stream, Line);
//    }
//
//    if(Line == Marker2) {
//        std::getline(Stream, Line);
//    }

    return Line;

}

