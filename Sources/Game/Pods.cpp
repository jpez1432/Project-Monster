
#include "Pods.hpp"

using namespace Game;

CPod::CPod(void)
{

}

CPod::~CPod(void)
{
    Directories.clear();
}

bool CPod::Load(std::string Filename)
{
    this->PodFile = Filename;

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.read((char*)&NumFiles, sizeof(NumFiles));
    StreamIn.read((char*)&Comment[0], sizeof(Comment));

    Directories.resize(NumFiles);
    StreamIn.read((char*)&Directories[0], sizeof(Directory) * NumFiles);

    StreamIn.close();

    return true;

}

CPodPool::CPodPool(void)
{

}

CPodPool::~CPodPool(void)
{
    Pods.clear();
}

bool CPodPool::Create(std::string Filename)
{

    unsigned int NumPods = 0;
    bool Resources = false;

    Filename.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(Filename.c_str(), std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn >> NumPods;
    Pods.resize(NumPods);

    for(unsigned int i = 0; i < Pods.size(); i++) {

        StreamIn >> Filename;

        if(Pods[i].Load(Filename)) {
            Logger->Text("Loaded Pod File Information - " + Filename);
        } else {
            Logger->Error("Error Loading Pod File Information - " + Filename);
        }

        if(StrCompare("Resources.pod", Filename.c_str())) {
            Resources = true;
        }

    }

    if(Resources == false) {
        Logger->Error("Error Loading Resources.pod, Please Ensure It Is Mounted...");
    }

    StreamIn.close();

    if(!EnumerateFiles()) {
        Logger->Error("No Trucks Found, Aborting...");
    }

    return true;

}

bool CPodPool::FindFile(std::string &Filename, int &Size, int &Offset)
{

    Size = Offset = 0;

    if(Config->Game.Pods == false) {
        return false;
    }

    for(unsigned int p = 0; p < Pods.size(); p++) {
        for(int i = 0; i < Pods[p].NumberOfFiles(); i++) {
            if(StrCompare(Pods[p].Filename(i), Filename.c_str())) {

                Filename = Pods[p].Filename();
                Size = Pods[p].Filesize(i);
                Offset = Pods[p].FileOffset(i);
                return true;

            }
        }
    }

    return false;

}

bool CPodPool::Prioritize(std::string Filename, int Element)
{

    for(unsigned int p = 0; p < Pods.size(); p++) {
        if(StrCompare(Pods[p].Filename().c_str(), "Resources.pod")) {
            std::swap(Pods[0], Pods[p]);
        }
    }

    for(unsigned int p = 0; p < Pods.size(); p++) {
        for(int i = 0; i < Pods[p].NumberOfFiles(); i++) {
            if(StrCompare(Pods[p].Filename(i), Filename.c_str())) {
                std::swap(Pods[Element], Pods[p]);
                return true;
            }
        }
    }

    return false;

}

bool CPodPool::EnumerateFiles(void)
{

    Info FileInfo;
    std::ifstream FileStream;
    std::string Line;
    std::vector<std::string> Files;

    Trucks.clear();
    Tracks.clear();

    if(Config->Game.Pods) {

        for(unsigned int p = 0; p < Pods.size(); p++) {
            for(int i = 0; i < Pods[p].NumberOfFiles(); i++) {

                if((strstr(Pods[p].Filename(i), ".trk") != NULL) || (strstr(Pods[p].Filename(i), ".TRK") != NULL)) {

                    FileStream.open(std::string(RootDir + ResourceDir + Pods[p].Filename()).c_str(), std::ios::in | std::ios::binary);
                    FileStream.seekg(Pods[p].FileOffset(i), std::ios::beg);

                    std::getline(FileStream, Line);
                    std::getline(FileStream, Line);
                    FileStream.close();

                    strcpy(FileInfo.Filename, strchr(Pods[p].Filename(i), '\\') + 1);
                    strcpy(FileInfo.Name, Line.c_str());

                    Trucks.push_back(FileInfo);
                }

                if((strstr(Pods[p].Filename(i), ".sit") != NULL) || (strstr(Pods[p].Filename(i), ".SIT") != NULL)) {

                    FileStream.open(std::string(RootDir + ResourceDir + Pods[p].Filename()).c_str(), std::ios::in | std::ios::binary);
                    FileStream.seekg(Pods[p].FileOffset(i), std::ios::beg);

                    std::getline(FileStream, Line);
                    std::getline(FileStream, Line);
                    std::getline(FileStream, Line);
                    FileStream.close();

                    strcpy(FileInfo.Filename, strchr(Pods[p].Filename(i), '\\') + 1);
                    strcpy(FileInfo.Name, Line.c_str());

                    Tracks.push_back(FileInfo);

                }
            }
        }
    }

    if(Config->Game.Files) {

        Files.clear();
        Files = ListFiles(RootDir + ResourceDir + "Truck\\", "*.trk");

        for(unsigned int i = 0; i < Files.size(); i++) {

            FileStream.open(std::string(RootDir + ResourceDir + "Truck\\" + Files[i]).c_str(), std::ios::in);

            std::getline(FileStream, Line);
            std::getline(FileStream, Line);
            FileStream.close();

            strcpy(FileInfo.Filename, Files[i].c_str());
            strcpy(FileInfo.Name, Line.c_str());

            Trucks.push_back(FileInfo);

        }

        Files.clear();
        Files = ListFiles(RootDir + ResourceDir + "World\\", "*.sit");

        for(unsigned int i = 0; i < Files.size(); i++) {

            FileStream.open(std::string(RootDir + ResourceDir + "World\\" + Files[i]).c_str(), std::ios::in);

            std::getline(FileStream, Line);
            std::getline(FileStream, Line);
            std::getline(FileStream, Line);
            FileStream.close();

            strcpy(FileInfo.Filename, Files[i].c_str());
            strcpy(FileInfo.Name, Line.c_str());

            Tracks.push_back(FileInfo);

        }
    }

    return true;

}
