
#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "Frustum.hpp"
#include "../Game/Track.hpp"
#include "../Game/Terrain.hpp"
#include "../Game/Textures.hpp"

namespace Engine
{

class CNode
{

public:

    CNode *Child[4];

    bool Leaf;
    bool Reflects;

    glm::ivec3 Center;
    glm::ivec3 Bounds[4];

    GLuint GLTerrain;
    GLuint GLBoxes;

public:

    CNode(void);
    ~CNode(void);

    void Render(bool Invert);

};

class CQuadTree
{

private:

    CNode *Root;
    Game::CTrack *Track;
    glm::ivec3 Bounds[4];
    unsigned int LeafSize;

    CNode *CreateNode(glm::ivec3 Bound[4]);
    void CloseNode(CNode *Node);

    void GetBoundingBox(glm::ivec3 Out[4], glm::ivec3 Offset, unsigned int Boxsize, unsigned int Index);

public:

    CQuadTree(void);
    ~CQuadTree(void);

    bool Create(Game::CTrack *Track, unsigned int LeafSize);
    void Destroy(void);

    CNode *GetRootNode(void)
    {
        return Root;
    }
    unsigned int &GetLeafSize(void)
    {
        return LeafSize;
    }

};
}

#endif
