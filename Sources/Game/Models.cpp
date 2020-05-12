

#include "Models.hpp"

using namespace Game;

CModel::CModel(void) : Min(65536), Max(-65536)
{

}

CModel::~CModel(void)
{
    Destroy();
}

bool CModel::Load(CPodPool &PodPool, std::string Filename, std::string Animated)
{

//    int Size, Offset;
//    std::string File;
//
//    int BlockID = -1;
//    int X, Y;
//    int NumVertices = 0;
//    int NumTextures = 0;
//    int TextureIndex = 0;
//    char Texture[16];
//    char AnimTexture[32];
//
//    glm::i8vec4 Color;
//    Face Split, Triangle;
//    Triangle.Vertices.resize(3);
//
//    File = Filename;
//    this->Filename = Filename;
//
//    File.insert(0, "Models\\");
//    PodPool.FindFile(File, Size, Offset);
//
//    if(Offset == 0 && !Config->Game.Files) {
//        return false;
//    }
//
//    File.insert(0, RootDir + ResourceDir);
//    std::ifstream StreamIn(File.c_str(), std::ios::binary | std::ios::in);
//
//    if(StreamIn.fail()) {
//        return false;
//    }
//
//    StreamIn.seekg(Offset, std::ios::beg);
//    StreamIn.read((char*)&ModelID, sizeof(ModelID));
//
//    if(ModelID == 0x20) {
//
//        StreamIn.seekg(20, std::ios::cur);
//
//        char Buffer[16];
//        StreamIn.read((char*)&Buffer, sizeof(Buffer));
//
//        Logger->Error("Animated Model, Loading First Frame - " + std::string(Buffer), true, false, false);
//
//        Load(PodPool, Buffer, Filename);
//
//        return true;
//
//    } else if(ModelID != 0x14) {
//
//        Logger->Error("Error Loading Model, Invalid Model ID Number - " + Filename);
//
//    }
//
//    if(Animated.length() != 0) {
//        this->Filename = Animated;
//    }
//
//    StreamIn.read((char*)&Header, sizeof(Header));
//
//    Buffer.resize(Header.NumVertices);
//    StreamIn.read((char*)&Buffer[0], sizeof(glm::i32vec3) * Header.NumVertices);
//
//    for(int i = 0; i < Header.NumVertices; i++) {
//        Vertices.push_back(glm::vec3(Buffer[i].x / 8192.0f, Buffer[i].y / 8192.0f, Buffer[i].z / -8192.0f));
//    }
//
//    Buffer.clear();
//
//    StreamIn.read((char*)&BlockID, sizeof(BlockID));
//
//    while(BlockID != 0 && !StreamIn.eof()) {
//
//        switch(BlockID) {
//
//        case 0x03:
//
//            StreamIn.seekg(sizeof(glm::i32vec3) * Header.NumVertices + 8, std::ios::cur);
//            break;
//
//        case 0x17:
//
//            StreamIn.seekg(8, std::ios::cur);
//            break;
//
//        case 0x0D:
//
//            StreamIn.seekg(4, std::ios::cur);
//            StreamIn.read((char*)&Texture[0], sizeof(char) * 16);
//            TextureIndex = Textures.size();
//
//            for(unsigned int i = 0; i < Textures.size(); i++) {
//                if(Textures[i] == Texture) {
//                    TextureIndex = i;
//                    break;
//                }
//            }
//
//            if(TextureIndex == (int)Textures.size()) {
//                Textures.push_back(std::string(Texture));
//                TextureIndex = Textures.size() - 1;
//            }
//
//            break;
//
//        case 0x1D:
//
//            StreamIn.seekg(4, std::ios::cur);
//            StreamIn.read((char*)&NumTextures, sizeof(int));
//            StreamIn.seekg(4 * sizeof(int), std::ios::cur);
//            StreamIn.read((char*)&AnimTexture[0], sizeof(char) * 32);
//            Textures.push_back(std::string(AnimTexture));
//            StreamIn.seekg(32 * (NumTextures - 1), std::ios::cur);
//
//            break;
//
//        case 0x0A:
//
//            StreamIn.read((char*)&Color, sizeof(glm::i8vec4));
//            Colors.push_back(glm::vec4(1.0f / Color[0], 1.0f / Color[1], 1.0f / Color[2], 1.0f / Color[3]));
//
//            break;
//
//        case 0x19:
//
//            Split.Type = BlockID;
//            Split.TextureIndex = TextureIndex;
//
//            StreamIn.read((char*)&NumVertices, sizeof(NumVertices));
//            StreamIn.seekg(4 * sizeof(int), std::ios::cur);
//
//            Split.Vertices.resize(NumVertices);
//
//            for(int i = 0; i < NumVertices; i++) {
//                StreamIn.read((char*)&Split.Vertices[i].Index, sizeof(Split.Vertices[i].Index));
//            }
//
//            if(NumVertices == 3) {
//
//                Faces.push_back(Split);
//
//            } else {
//
//                Triangle.Type = BlockID;
//                Triangle.TextureIndex = Colors.size() - 1;
//
//                Triangle.Vertices[0].Index = Split.Vertices[0].Index;
//                Triangle.Vertices[1].Index = Split.Vertices[1].Index;
//                Triangle.Vertices[2].Index = Split.Vertices[2].Index;
//                Faces.push_back(Triangle);
//
//                Triangle.Vertices[0].Index = Split.Vertices[2].Index;
//                Triangle.Vertices[1].Index = Split.Vertices[3].Index;
//                Triangle.Vertices[2].Index = Split.Vertices[0].Index;
//                Faces.push_back(Triangle);
//
//            }
//
//            break;
//
//        case 0x0E:
//        case 0x11:
//        case 0x18:
//        case 0x29:
//        case 0x33:
//        case 0x34:
//
//            Split.Type = BlockID;
//            Split.TextureIndex = TextureIndex;
//
//            StreamIn.read((char*)&NumVertices, sizeof(NumVertices));
//            StreamIn.seekg(4 * sizeof(int), std::ios::cur);
//
//            Split.Vertices.resize(NumVertices);
//
//            for(int i = 0; i < NumVertices; i++) {
//                StreamIn.read((char*)&Split.Vertices[i].Index, sizeof(Split.Vertices[i].Index));
//                StreamIn.read((char*)&X, sizeof(X));
//                StreamIn.read((char*)&Y, sizeof(Y));
//                Split.Vertices[i].TexCoord = glm::vec2(X / float(65536.0f * 256.0f), Y / float(65536.0f * 256.0f));
////                Split.Vertices[i].TexCoord = glm::vec2(X / float(65280.0f * 256.0f), Y / float(65280.0f * 256.0f));
//            }
//
//            if(NumVertices == 3) {
//
//                Faces.push_back(Split);
//
//            } else {
//
//                Triangle.Type = Split.Type;
//                Triangle.TextureIndex = Split.TextureIndex;
//
//                Triangle.Vertices[0] = Split.Vertices[0];
//                Triangle.Vertices[1] = Split.Vertices[1];
//                Triangle.Vertices[2] = Split.Vertices[2];
//                Faces.push_back(Triangle);
//
//                Triangle.Vertices[0] = Split.Vertices[2];
//                Triangle.Vertices[1] = Split.Vertices[3];
//                Triangle.Vertices[2] = Split.Vertices[0];
//                Faces.push_back(Triangle);
//
//            }
//
//            break;
//
//        default:
//
//            Logger->Error("Unknown Model Face Type - " + std::to_string(BlockID) + ", Skipping...", true, true, false);
//
//            break;
//
//        }
//
//        StreamIn.read((char*)&BlockID, sizeof(BlockID));
//
//    }
//
//    StreamIn.close();
    int Size, Offset;
    std::string File;

    int BlockID = -1;
    int X, Y;
    int NumVertices = 0;
    int NumTextures = 0;
    int TextureIndex = 0;
    char Texture[16];
    char AnimTexture[32];

    glm::i8vec4 Color;
    Face Split, Triangle;

    Split.Neighbors = glm::uvec3(0);
    Triangle.Vertices.resize(3);
    Triangle.Neighbors = glm::uvec3(0);

    File = Filename;
    this->Filename = Filename;

    File.insert(0, "Models\\");
    PodPool.FindFile(File, Size, Offset);

    if(Offset == 0 && !Config->Game.Files) {
        return false;
    }

    File.insert(0, RootDir + ResourceDir);
    std::ifstream StreamIn(File.c_str(), std::ios::binary | std::ios::in);

    if(StreamIn.fail()) {
        return false;
    }

    StreamIn.seekg(Offset, std::ios::beg);
    StreamIn.read((char*)&ModelID, sizeof(ModelID));

    if(ModelID == 0x20) {

        StreamIn.seekg(20, std::ios::cur);

        char Buffer[16];
        StreamIn.read((char*)&Buffer, sizeof(Buffer));

        Logger->Error("Animated Model, Loading First Frame - " + std::string(Buffer), true, false, false);

        Load(PodPool, Buffer, Filename);

        return true;

    } else if(ModelID != 0x14) {

        Logger->Error("Error Loading Model, Invalid Model ID Number - " + Filename);

    }

    if(Animated.length() != 0) {
        this->Filename = Animated;
    }

    StreamIn.read((char*)&Header, sizeof(Header));

    Buffer.resize(Header.NumVertices);
    StreamIn.read((char*)&Buffer[0], sizeof(glm::i32vec3) * Header.NumVertices);

    for(int i = 0; i < Header.NumVertices; i++) {
        Vertices.push_back(glm::vec3(Buffer[i].x / 8192.0f, Buffer[i].y / 8192.0f, Buffer[i].z / -8192.0f));
    }

    Buffer.clear();

    StreamIn.read((char*)&BlockID, sizeof(BlockID));

    while(BlockID != 0 && !StreamIn.eof()) {

        switch(BlockID) {

        case 0x03:

            StreamIn.seekg(sizeof(glm::i32vec3) * Header.NumVertices + 8, std::ios::cur);
            break;

        case 0x17:

            StreamIn.seekg(8, std::ios::cur);
            break;

        case 0x0D:

            StreamIn.seekg(4, std::ios::cur);
            StreamIn.read((char*)&Texture[0], sizeof(char) * 16);
            TextureIndex = Textures.size();

            for(unsigned int i = 0; i < Textures.size(); i++) {
                if(Textures[i] == Texture) {
                    TextureIndex = i;
                    break;
                }
            }

            if(TextureIndex == (int)Textures.size()) {
                Textures.push_back(std::string(Texture));
                TextureIndex = Textures.size() - 1;
            }

            break;

        case 0x1D:

            StreamIn.seekg(4, std::ios::cur);
            StreamIn.read((char*)&NumTextures, sizeof(int));
            StreamIn.seekg(4 * sizeof(int), std::ios::cur);
            StreamIn.read((char*)&AnimTexture[0], sizeof(char) * 32);
            Textures.push_back(std::string(AnimTexture));
            StreamIn.seekg(32 * (NumTextures - 1), std::ios::cur);

            break;

        case 0x0A:

            StreamIn.read((char*)&Color, sizeof(glm::i8vec4));
            Colors.push_back(glm::vec4(1.0f / Color[0], 1.0f / Color[1], 1.0f / Color[2], 1.0f / Color[3]));

            break;

        case 0x19:

            Split.Type = BlockID;
            Split.TextureIndex = TextureIndex;

            StreamIn.read((char*)&NumVertices, sizeof(NumVertices));
            StreamIn.seekg(4 * sizeof(int), std::ios::cur);

            Split.Vertices.resize(NumVertices);

            for(int i = 0; i < NumVertices; i++) {
                StreamIn.read((char*)&Split.Vertices[i].Index, sizeof(Split.Vertices[i].Index));
            }

            if(NumVertices == 3) {

                Faces.push_back(Split);

            } else {

                Triangle.Type = BlockID;
                Triangle.TextureIndex = Colors.size() - 1;

                Triangle.Vertices[0].Index = Split.Vertices[0].Index;
                Triangle.Vertices[1].Index = Split.Vertices[1].Index;
                Triangle.Vertices[2].Index = Split.Vertices[2].Index;
                Faces.push_back(Triangle);

                Triangle.Vertices[0].Index = Split.Vertices[2].Index;
                Triangle.Vertices[1].Index = Split.Vertices[3].Index;
                Triangle.Vertices[2].Index = Split.Vertices[0].Index;
                Faces.push_back(Triangle);

            }

            break;

        case 0x0E:
        case 0x11:
        case 0x18:
        case 0x29:
        case 0x33:
        case 0x34:

            Split.Type = BlockID;
            Split.TextureIndex = TextureIndex;

            StreamIn.read((char*)&NumVertices, sizeof(NumVertices));
            StreamIn.seekg(4 * sizeof(int), std::ios::cur);

            Split.Vertices.resize(NumVertices);

            for(int i = 0; i < NumVertices; i++) {
                StreamIn.read((char*)&Split.Vertices[i].Index, sizeof(Split.Vertices[i].Index));
                StreamIn.read((char*)&X, sizeof(X));
                StreamIn.read((char*)&Y, sizeof(Y));
                Split.Vertices[i].TexCoord = glm::vec2(X / float(65536.0f * 256.0f), Y / float(65536.0f * 256.0f));
//                Split.Vertices[i].TexCoord = glm::vec2(X / float(65280.0f * 256.0f), Y / float(65280.0f * 256.0f));
            }

            if(NumVertices == 3) {

                Faces.push_back(Split);

            } else {

                Triangle.Type = Split.Type;
                Triangle.TextureIndex = Split.TextureIndex;

                Triangle.Vertices[0] = Split.Vertices[0];
                Triangle.Vertices[1] = Split.Vertices[1];
                Triangle.Vertices[2] = Split.Vertices[2];
                Faces.push_back(Triangle);

                Triangle.Vertices[0] = Split.Vertices[2];
                Triangle.Vertices[1] = Split.Vertices[3];
                Triangle.Vertices[2] = Split.Vertices[0];
                Faces.push_back(Triangle);

            }

            break;

        default:

            Logger->Error("Unknown Model Face Type - " + std::to_string(BlockID) + ", Skipping...", true, true, false);

            break;

        }

        StreamIn.read((char*)&BlockID, sizeof(BlockID));

    }

    StreamIn.close();

    CalculateBounds(Min, Max);
    CalculateNormals();
    CalculateConnectivity();
    CalculatePlanes(glm::vec3(0.0f));

    if(TexturePool.Create(PodPool, Textures)) {
        Logger->Text("Create Model Texture Pool");
    } else {
        Logger->Error("Error Creating Model Texture Pool");
    }

    Build();

    return true;

}

void CModel::Destroy(void)
{
    Vertices.clear();
    Normals.clear();
    Textures.clear();
    AnimatedTextures.clear();
    Colors.clear();
    Faces.clear();

    if(glIsList(GLModel)) {
        glDeleteLists(GLModel, 1);
        Logger->Text("Released Model OpenGL Display List Memory - " + Filename);
    }
}

bool CModel::Build(void)
{
    GLModel = glGenLists(1);
    glNewList(GLModel, GL_COMPILE);

    for(unsigned int t = 0; t < Textures.size(); t++) {

        TexturePool.Bind(t);

//        glEnable(GL_CULL_FACE);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
//        glCullFace(GL_BACK);
//        glEnable(GL_CULL_FACE);

        glBegin(GL_TRIANGLES);

        for(unsigned int f = 0; f < Faces.size(); f++) {

            if((Faces[f].Type == 0x0E) || (Faces[f].Type == 0x18) /*|| (Faces[f].Type == 0x19)*/ || (Faces[f].Type == 0x29) || (Faces[f].Type == 0x34)) {

                if(Faces[f].TextureIndex == int(t)) {

                    for(int v = 0; v < 3; v++) {

                        glTexCoord2fv(&Faces[f].Vertices[v].TexCoord[0]);
                        glNormal3fv(&Normals[Faces[f].Vertices[v].Index][0]);
                        glVertex3fv(&Vertices[Faces[f].Vertices[v].Index][0]);

                    }
                }
            }
        }

        glEnd();

        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GEQUAL, 0.75f);
//        glDisable(GL_CULL_FACE);
//        glCullFace(GL_BACK);
        glBegin(GL_TRIANGLES);

        for(unsigned int f = 0; f < Faces.size(); f++) {

            if((Faces[f].Type == 0x11) || (Faces[f].Type == 0x33)) {

                if(Faces[f].TextureIndex == int(t)) {

                    for(int v = 0; v < 3; v++) {

                        glTexCoord2fv(&Faces[f].Vertices[v].TexCoord[0]);
                        glNormal3fv(&Normals[Faces[f].Vertices[v].Index][0]);
                        glVertex3fv(&Vertices[Faces[f].Vertices[v].Index][0]);

                    }
                }
            }
        }

        glEnd();

    }

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBegin(GL_TRIANGLES);

    for(unsigned int f = 0; f < Faces.size(); f++) {

        if(Faces[f].Type == 0x19 /*== 0x0A*/) {

            for(int v = 0; v < 3; v++) {

                glColor4fv(&Colors[Faces[f].TextureIndex][0]);
                glNormal3fv(&Normals[Faces[f].Vertices[v].Index][0]);
                glVertex3fv(&Vertices[Faces[f].Vertices[v].Index][0]);

            }
        }

    }

    glEnd();
    glEndList();

    return glIsList(GLModel);

}

void CModel::CalculateConnectivity(void)
{
    unsigned int p1i, p2i, p1j, p2j;
    unsigned int P1i, P2i, P1j, P2j;

    for (unsigned int i = 0 ; i < Faces.size() - 1; i++) {
        for (unsigned int j = i + 1; j < Faces.size(); j++) {
            for (unsigned int ki = 0; ki < 3; ki++) {
                if (!Faces[i].Neighbors[ki]) {
                    for (unsigned int kj = 0; kj < 3; kj++) {

                        p1i = ki;
                        p1j = kj;
                        p2i = (ki + 1) % 3;
                        p2j = (kj + 1) % 3;

                        p1i = Faces[i].Vertices[p1i].Index;
                        p2i = Faces[i].Vertices[p2i].Index;
                        p1j = Faces[i].Vertices[p1j].Index;
                        p2j = Faces[i].Vertices[p2j].Index;

                        P1i = ((p1i + p2i) - std::abs(int(p1i - p2i))) / 2;
                        P2i = ((p1i + p2i) + std::abs(int(p1i - p2i))) / 2;
                        P1j = ((p1j + p2j) - std::abs(int(p1j - p2j))) / 2;
                        P2j = ((p1j + p2j) + std::abs(int(p1j - p2j))) / 2;

                        if ((P1i == P1j) && (P2i == P2j)) {
                            Faces[i].Neighbors[ki] = j + 1;
                            Faces[j].Neighbors[kj] = i + 1;
                        }
                    }
                }
            }
        }
    }
}

void CModel::CalculatePlanes(glm::vec3 Position)
{
    for(unsigned int i = 0; i < Faces.size(); i++) {

        glm::vec3 Vertex[4];

        for(unsigned int j = 0; j < 3; j++) {
            Vertex[j + 1].x = Vertices[Faces[i].Vertices[j].Index].x + Position.x;
            Vertex[j + 1].y = Vertices[Faces[i].Vertices[j].Index].y + Position.y;
            Vertex[j + 1].z = Vertices[Faces[i].Vertices[j].Index].z + Position.z;
        }

        Faces[i].PlaneEq.x = Vertex[1].y * (Vertex[2].z - Vertex[3].z) + Vertex[2].y * (Vertex[3].z - Vertex[1].z) + Vertex[3].y * (Vertex[1].z - Vertex[2].z);
        Faces[i].PlaneEq.y = Vertex[1].z * (Vertex[2].x - Vertex[3].x) + Vertex[2].z * (Vertex[3].x - Vertex[1].x) + Vertex[3].z * (Vertex[1].x - Vertex[2].x);
        Faces[i].PlaneEq.z = Vertex[1].x * (Vertex[2].y - Vertex[3].y) + Vertex[2].x * (Vertex[3].y - Vertex[1].y) + Vertex[3].x * (Vertex[1].y - Vertex[2].y);
        Faces[i].PlaneEq.w = -(Vertex[1].x * (Vertex[2].y * Vertex[3].z - Vertex[3].y * Vertex[2].z) +
                               Vertex[2].x * (Vertex[3].y * Vertex[1].z - Vertex[1].y * Vertex[3].z) +
                               Vertex[3].x * (Vertex[1].y * Vertex[2].z - Vertex[2].y * Vertex[1].z));
    }
}

void CModel::CalculateNormals(void)
{

    glm::vec3 N, A, B;
    std::vector<int> SharedFaces;

    Normals.resize(Vertices.size());
    SharedFaces.resize(Vertices.size());

    for(unsigned int f = 0; f < Faces.size(); f++) {

        A = Vertices[Faces[f].Vertices[2].Index] - Vertices[Faces[f].Vertices[0].Index];
        B = Vertices[Faces[f].Vertices[1].Index] - Vertices[Faces[f].Vertices[0].Index];

        N = glm::cross(B, A);
        N = glm::normalize(N);

        FaceNormals.push_back(N);

        Normals[Faces[f].Vertices[0].Index] += N;
        Normals[Faces[f].Vertices[1].Index] += N;
        Normals[Faces[f].Vertices[2].Index] += N;

        SharedFaces[Faces[f].Vertices[0].Index]++;
        SharedFaces[Faces[f].Vertices[1].Index]++;
        SharedFaces[Faces[f].Vertices[2].Index]++;
    }

    for(unsigned int i = 0; i < Vertices.size(); i++) {
        Normals[i] /= SharedFaces[i];
        Normals[i] = glm::normalize(Normals[i]);
    }

}

float CModel::CalculateBounds(glm::vec3 &Min, glm::vec3 &Max)
{
    BoundingSphere = 0.0f;
    Min = Max = Center = glm::vec3(0);

    for(unsigned int v = 0; v < Vertices.size(); v++) {

        Center += Vertices[v];

        if(Vertices[v].x < Min.x) {
            Min.x = Vertices[v].x;
        }

        if(Vertices[v].y < Min.y) {
            Min.y = Vertices[v].y;
        }

        if(Vertices[v].z < Min.z) {
            Min.z = Vertices[v].z;
        }

        if(Vertices[v].x > Max.x) {
            Max.x = Vertices[v].x;
        }

        if(Vertices[v].y > Max.y) {
            Max.y = Vertices[v].y;
        }

        if(Vertices[v].z > Max.z) {
            Max.z = Vertices[v].z;
        }

    }

    Center /= Vertices.size();

    glm::vec3 Dimensions = (Max - Min);

    BoundingSphere = 0;
    if (Dimensions.x > BoundingSphere) BoundingSphere = Dimensions.x;
    if (Dimensions.y > BoundingSphere) BoundingSphere = Dimensions.y;
    if (Dimensions.z > BoundingSphere) BoundingSphere = Dimensions.z;

//    BoundingSphere = 0;
//    if (abs(Min.x) > BoundingSphere) BoundingSphere = abs(Min.x);
//    if (abs(Min.y) > BoundingSphere) BoundingSphere = abs(Min.y);
//    if (abs(Min.z) > BoundingSphere) BoundingSphere = abs(Min.z);
//    if (Max.x > BoundingSphere) BoundingSphere = Max.x;
//    if (Max.y > BoundingSphere) BoundingSphere = Max.y;
//    if (Max.z > BoundingSphere) BoundingSphere = Max.z;

    return BoundingSphere;

//    BoundingSphere = 0.0f;
//    Min = Max = glm::vec3(0);
//
//    for(unsigned int v = 0; v < Vertices.size(); v++) {
//
//        if(Vertices[v].x < Min.x) {
//            Min.x = Vertices[v].x;
//        }
//
//        if(Vertices[v].y < Min.y) {
//            Min.y = Vertices[v].y;
//        }
//
//        if(Vertices[v].z < Min.z) {
//            Min.z = Vertices[v].z;
//        }
//
//        if(Vertices[v].x > Max.x) {
//            Max.x = Vertices[v].x;
//        }
//
//        if(Vertices[v].y > Max.y) {
//            Max.y = Vertices[v].y;
//        }
//
//        if(Vertices[v].z > Max.z) {
//            Max.z = Vertices[v].z;
//        }
//
//    }
//
//    glm::vec3 Dimensions = (Max - Min);
//    BoundingSphere = float(Dimensions.x + Dimensions.y + Dimensions.z) * 1.5f;
//
//    return BoundingSphere;

}

glm::vec3 CModel::GetBounds(glm::vec3 &Min, glm::vec3 &Max)
{
    Min = this->Min;
    Max = this->Max;

    return Max - Min;
}


CModelPool::CModelPool(void)
{

}

CModelPool::~CModelPool(void)
{
    Models.clear();
}

bool CModelPool::Create(CPodPool &PodPool, std::vector<std::string> &Filenames)
{
    Models.resize(Filenames.size());

    for(unsigned int i = 0; i < Models.size(); i++) {

        if(Models[i].Load(PodPool, Filenames[i])) {
            Logger->Text("Loaded Bin Model - " + Filenames[i]);
        } else {
            Logger->Error("Error Loading Bin Model - " + Filenames[i]);
        }

    }

    return true;

}
