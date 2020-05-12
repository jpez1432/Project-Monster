
#ifndef GENERIC_HPP
#define GENERIC_HPP

#define dDOUBLE

#include <Ode/Ode.h>

#include "Client.hpp"
#include "../Game/Models.hpp"

void ODEtoOGL(double *Matrix, const dReal *Position, const dReal *Rotation);
void OGLtoODE(const double *Matrix, const dReal *Position, dReal *Rotation);

namespace Physics
{

class CGeneric : public CClient
{

public:

    int Type;
    unsigned char ModelType;

    dMass Mass;
    dBodyID Body;
    dGeomID Geom;

    double Matrix[16];

    GLuint GLModel;
    Game::CModel *Model;

public:

    CGeneric(void);
    virtual ~CGeneric(void);

    virtual bool Update(dReal Step);
    virtual void Render(void);

    bool Create(int Type, Game::CModel &Model, bool Static = true, unsigned char ModelType = 0, dReal Width = 0.0, dReal Height = 0.0, dReal Length = 0.0, dReal Radius = 0.0, dReal Mass = 1.0);

    const glm::vec3 GetPosition(void)
    {
        const dReal *Pos = dGeomGetPosition(Geom);
        return glm::vec3(Pos[0], Pos[1], Pos[2]);
    }
    double *GetMatrix(void) {
        return Matrix;
    }

    void SetMass(dReal Mass);
    void SetData(GeomData *Data);
    void SetEnviroment(void);
    void SetPosition(dReal X, dReal Y, dReal Z);
    void SetRotation(dReal X, dReal Y, dReal Z);

};
}

#endif
