
#include "QuadTree.hpp"

using namespace Engine;

CNode::CNode(void) : Leaf(false), Reflects(false), GLTerrain(0), GLBoxes(0)
{
    for (int i = 0; i < 4; i++) {
        Child[i] = NULL;
    }
}

CNode::~CNode(void)
{
    if (Leaf) {
        glDeleteLists(GLTerrain, 1);
        glDeleteLists(GLBoxes, 1);
    }
}

void CNode::Render(bool Invert)
{
    if (Invert) {

        glCullFace(GL_FRONT);

        if (Config->Render.Terrain && Reflects) {
            glCallList(GLTerrain);
        }

        if (Config->Render.Boxes) {
            glCallList(GLBoxes);
        }

    } else {

        glCullFace(GL_BACK);

        if (Config->Render.Terrain) {
            glCallList(GLTerrain);
        }

        if (Config->Render.Boxes) {
            glCallList(GLBoxes);
        }

    }
}

CQuadTree::CQuadTree(void) : Root(NULL), LeafSize(0)
{


}

CQuadTree::~CQuadTree(void)
{
    Destroy();
}

CNode *CQuadTree::CreateNode(glm::ivec3 Bound[4])
{

    CNode *Node = new CNode();
    Node->Center = glm::ivec3((Bound[3].x - Bound[0].x) / 2 + Bound[0].x, (Bound[3].y - Bound[0].y) / 2 + Bound[0].y, (Bound[3].z - Bound[0].z) / 2 + Bound[0].z); ;
    memcpy(Node->Bounds, Bound, sizeof(Bound[0]) * 4);

    if (Bound[1].x - Bound[0].x == int(LeafSize)) {

        Node->Leaf = true;

        int Rotation;
        int MirrorX, MirrorY;
        int OffsetX, OffsetZ;
        float Lower, Upper;

        std::vector<unsigned int> Duplicates;
        Game::CTexturePool *Pool = Track->TexturePoolObject();
        Game::CTerrain *Terrain = Track->TerrainObject();
        Game::CBoxes *Boxes = Track->BoxesObject();

        for (unsigned int x = 0; x < LeafSize; x++) {
            for (unsigned int z = 0; z < LeafSize; z++) {

                OffsetX = (Node->Center.x - LeafSize / 2) + x;
                OffsetZ = (Node->Center.z - LeafSize / 2) + z;
                Duplicates.push_back(Terrain->GetTextureIndex(OffsetX, OffsetZ));

                if (Terrain->GetNormal(OffsetX, OffsetZ) != glm::vec3(0.0f, -1.0f, 0.0f) || (Terrain->GetHeight(OffsetX, OffsetZ).y > Track->GetWaterHeight())) {
                    Node->Reflects = true;
                }
            }
        }

        std::sort(Duplicates.begin(), Duplicates.end());
        Duplicates.erase(std::unique(Duplicates.begin(), Duplicates.end()), Duplicates.end());

        Node->GLTerrain = glGenLists(1);
        glNewList(Node->GLTerrain, GL_COMPILE);

        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);

        for (unsigned int i = 0; i < Duplicates.size(); i++) {

            Pool->Bind(Duplicates[i]);

            glBegin(GL_QUADS);

            glFrontFace(GL_CW);

            for (unsigned int x = 0; x < LeafSize; x++) {
                for (unsigned int z = 0; z < LeafSize; z++) {

                    OffsetX = (Node->Center.x - LeafSize / 2) + x;
                    OffsetZ = (Node->Center.z - LeafSize / 2) + z;

                    if (Terrain->GetTextureIndex(OffsetX, OffsetZ) == Duplicates[i]) {

                        Terrain->GetTextureRotation(OffsetX, OffsetZ, MirrorX, MirrorY, Rotation);

                        glNormal3fv(&Terrain->GetNormal(OffsetX, OffsetZ)[0]);
                        glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                        glVertex3fv(&Terrain->GetHeight(OffsetX, OffsetZ)[0]);

                        glNormal3fv(&Terrain->GetNormal(OffsetX + 1, OffsetZ)[0]);
                        glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                        glVertex3fv(&Terrain->GetHeight(OffsetX + 1, OffsetZ)[0]);

                        glNormal3fv(&Terrain->GetNormal(OffsetX + 1, OffsetZ + 1)[0]);
                        glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                        glVertex3fv(&Terrain->GetHeight(OffsetX + 1, OffsetZ + 1)[0]);

                        glNormal3fv(&Terrain->GetNormal(OffsetX, OffsetZ + 1)[0]);
                        glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                        glVertex3fv(&Terrain->GetHeight(OffsetX, OffsetZ + 1)[0]);

                    }
                }
            }

            glEnd();

        }

        glFrontFace(GL_CW);

        glEndList();


        Node->GLBoxes = glGenLists(1);
        glNewList(Node->GLBoxes, GL_COMPILE);

        glFrontFace(GL_CW);

        for (unsigned int x = 0; x < LeafSize; x++) {
            for (unsigned int z = 0; z < LeafSize; z++) {

                OffsetX = (Node->Center.x - LeafSize / 2) + x;
                OffsetZ = (Node->Center.z - LeafSize / 2) + z;

                Lower = Boxes->GetLowerHeight(OffsetX, OffsetZ);
                Upper = Boxes->GetUpperHeight(OffsetX, OffsetZ);

                if (Lower != Upper) {

                    Boxes->GetTextureRotation(OffsetX, OffsetZ, 0, MirrorX, MirrorY, Rotation);
                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 0));
                    glBegin(GL_QUADS);

                    glNormal3f(1, 0, 0);
                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ + 1);

                    glEnd();

                    Boxes->GetTextureRotation(OffsetX, OffsetZ, 1, MirrorX, MirrorY, Rotation);
                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 1));
                    glBegin(GL_QUADS);

                    glNormal3f(-1, 0, 0);
                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    glEnd();

                    Boxes->GetTextureRotation(OffsetX, OffsetZ, 2, MirrorX, MirrorY, Rotation);
                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 2));
                    glBegin(GL_QUADS);

                    glNormal3f(0, 0, -1);
                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                    glEnd();

                    Boxes->GetTextureRotation(OffsetX, OffsetZ, 3, MirrorX, MirrorY, Rotation);
                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 3));
                    glBegin(GL_QUADS);

                    glNormal3f(0, 0, 1);
                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ);

                    glEnd();

                    Boxes->GetTextureRotation(OffsetX, OffsetZ, 4, MirrorX, MirrorY, Rotation);
                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 4));
                    glBegin(GL_QUADS);

                    glNormal3f(0, -1, 0);
                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Upper, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Upper, OffsetZ + 1);

                    glEnd();

                    Boxes->GetTextureRotation(OffsetX, OffsetZ, 5, MirrorX, MirrorY, Rotation);
                    Pool->Bind(Boxes->GetTextureIndex(OffsetX, OffsetZ, 5));
                    glBegin(GL_QUADS);

                    glNormal3f(0, 1, 0);
                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 0)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 3)[0]);
                    glVertex3f(OffsetX, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 2)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ + 1);

                    glTexCoord2fv(&Pool->GetTextureCoords(MirrorX, MirrorY, Rotation, 1)[0]);
                    glVertex3f(OffsetX + 1, Lower, OffsetZ);

                    glEnd();

                }

            }
        }

        glFrontFace(GL_CW);

        glEndList();

    } else {

        glm::ivec3 Bounds[4];

        for (int i = 0; i < 4; i++) {

            GetBoundingBox(Bounds, Bound[0], (Bound[1].x - Bound[0].x) / 2, i);
            Node->Child[i] = CreateNode(Bounds);

        }

    }

    return Node;

}

void CQuadTree::Destroy(void)
{
    if (Root) {
        CloseNode(Root);
        Root = NULL;
    }
}

void CQuadTree::GetBoundingBox(glm::ivec3 Out[4], glm::ivec3 Offset, unsigned int Boxsize, unsigned int Index)
{
    glm::ivec3 Shift(0);

    if (Index == 1) {
        Shift = glm::ivec3(Boxsize, 0, 0);
    } else if (Index == 2) {
        Shift = glm::ivec3(0, 0, Boxsize);
    } else if (Index == 3) {
        Shift = glm::ivec3(Boxsize, 0, Boxsize);
    }

    Out[0] = Offset	+ glm::ivec3(0, 0, 0) + Shift;
    Out[1] = Offset + glm::ivec3(Boxsize, 0, 0) + Shift;
    Out[2] = Offset + glm::ivec3(0, 0, Boxsize) + Shift;
    Out[3] = Offset + glm::ivec3(Boxsize, 0, Boxsize) + Shift;

}

void CQuadTree::CloseNode(CNode *Node)
{
    for (unsigned int i = 0; i < 4; i++) {

        if (Node->Child[i]) {

            CloseNode(Node->Child[i]);
            delete Node->Child[i];
            Node->Child[i] = NULL;

        }

    }

}

bool CQuadTree::Create(Game::CTrack *Track, unsigned int LeafSize)
{
    Destroy();

    this->Track = Track;
    this->LeafSize = LeafSize;

    GetBoundingBox(Bounds, glm::ivec3(0), Track->Dimensions(), 0);
    Root = CreateNode(Bounds);

    if (Root) {
        return true;
    }

    return false;

}










