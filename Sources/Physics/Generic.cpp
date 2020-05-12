
#include "Generic.hpp"

using namespace Physics;

void ODEtoOGL(double *Matrix, const dReal *Position, const dReal *Rotation)
{
    Matrix[0]  = Rotation[0];
    Matrix[1]  = Rotation[4];
    Matrix[2]  = Rotation[8];
    Matrix[3]  = 0.0f;
    Matrix[4]  = Rotation[1];
    Matrix[5]  = Rotation[5];
    Matrix[6]  = Rotation[9];
    Matrix[7]  = 0.0f;
    Matrix[8]  = Rotation[2];
    Matrix[9]  = Rotation[6];
    Matrix[10] = Rotation[10];
    Matrix[11] = 0.0f;
    Matrix[12] = Position[0];
    Matrix[13] = Position[1];
    Matrix[14] = Position[2];
    Matrix[15] = 1.0f;
}

void OGLtoODE(const double *Matrix, const dReal *Position, dReal *Rotation)
{
    Rotation[0]  = Matrix[0];
    Rotation[4]  = Matrix[1];
    Rotation[8]  = Matrix[2];
    Rotation[3]  = 0.0f;
    Rotation[1]  = Matrix[4];
    Rotation[5]  = Matrix[5];
    Rotation[9]  = Matrix[6];
    Rotation[7]  = 0.0f;
    Rotation[2]  = Matrix[8];
    Rotation[6]  = Matrix[9];
    Rotation[10] = Matrix[10];
    Rotation[11] = 0.0f;
    Rotation[12] = Position[0];
    Rotation[13] = Position[1];
    Rotation[14] = Position[2];
    Rotation[15] = 1.0f;
}

CGeneric::CGeneric(void) : Body(0), Geom(0)
{
    EnvData.Slip = 0.0000006f;
}

CGeneric::~CGeneric(void)
{

    if(Body) {
        dBodyDestroy(Body);
        Body = NULL;
    }

    if(Geom) {
        dGeomDestroy(Geom);
        Geom = NULL;
    }



}

bool CGeneric::Update(dReal Step)
{

//    if(Body == 0 || Geom == 0) {
//        return false;
//    }

//    Render();

    return true;

}

void CGeneric::Render(void)
{

    if(Geom == 0) {
        return;
    }


    dVector3 Sides;
    dReal Radius;
    dReal Width, Height, Length;

    GLUquadric *Object = gluNewQuadric();

    const dReal *Position = dGeomGetPosition(Geom);
    const dReal *Rotation = dGeomGetRotation(Geom);

    ODEtoOGL(Matrix, Position, Rotation);

    glPushMatrix();

    glMultMatrixd(Matrix);

    switch(Type) {

    case dSphereClass:

        Radius = dGeomSphereGetRadius(Geom);
        gluSphere(Object, Radius, 10, 10);

        break;

    case dBoxClass:

        dGeomBoxGetLengths(Geom, Sides);

        Width = Sides[0] * 0.5f;
        Height = Sides[1] * 0.5f;
        Length = Sides[2] * 0.5f;

        glBegin(GL_TRIANGLE_STRIP);

        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-Width, -Height, -Length);
        glVertex3f(-Width, -Height, Length);
        glVertex3f(-Width, Height, -Length);
        glVertex3f(-Width, Height, Length);

        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(Width, Height, -Length);
        glVertex3f(Width, Height, Length);

        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(Width, -Height, -Length);
        glVertex3f(Width, -Height, Length);

        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-Width, -Height, -Length);
        glVertex3f(-Width, -Height, Length);

        glEnd();

        glBegin(GL_TRIANGLE_FAN);

        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-Width, -Height, Length);
        glVertex3f(Width, -Height, Length);
        glVertex3f(Width, Height, Length);
        glVertex3f(-Width, Height, Length);

        glEnd();

        glBegin(GL_TRIANGLE_FAN);

        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-Width, -Height, -Length);
        glVertex3f(-Width, Height, -Length);
        glVertex3f(Width, Height, -Length);
        glVertex3f(Width, -Height, -Length);

        glEnd();

        break;

    case dCCylinderClass:

        dGeomCCylinderGetParams(Geom, &Radius, &Length);
        gluCylinder(Object, Radius, Radius, Length, 10, 10);

        glPushMatrix();
        glTranslatef(0, 0, -Length / 2 + Radius);
        gluSphere(Object, Radius, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, Length / 2 + Radius);
        gluSphere(Object, Radius, 10, 10);
        glPopMatrix();

        break;

    case dCylinderClass:

        dGeomCylinderGetParams(Geom, &Radius, &Length);
        gluCylinder(Object, Radius, Radius, Length, 10, 10);

        glPushMatrix();
        glTranslatef(0, 0, -Length / 2 + Radius);
        gluDisk(Object, 0, Radius, 10, 10);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, Length / 2 + Radius);
        gluDisk(Object, 0, Radius, 10, 10);
        glPopMatrix();

        break;

    case dTriMeshClass:

        glCallList(GLModel);

        break;


    default:

        return;

    }

    glPopMatrix();

    gluDeleteQuadric(Object);

}

bool CGeneric::Create(int Type, Game::CModel &Model , bool Static, unsigned char ModelType, dReal Width, dReal Height, dReal Length, dReal Radius, dReal Mass)
{
    if(World == 0 || Space == 0) {
        return false;
    }

    this->Type = Type;
    this->ModelType = ModelType;
    this->Model = &Model;

    dMassAdjust(&this->Mass, Mass);

    switch(Type) {

    case dSphereClass:

        Geom = dCreateSphere(0, Radius);
        SetData(&EnvData);

        if(!Static) {
            Body = dBodyCreate(World);
            dMassSetSphere(&this->Mass, 1, Radius);
            dBodySetMass(Body, &this->Mass);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dBoxClass:

        Geom = dCreateBox(0, Width, Height, Length);
        SetData(&EnvData);

        if(!Static) {
            Body = dBodyCreate(World);
            dMassSetBox(&this->Mass, 1, Width, Height, Length);
            dBodySetMass(Body, &this->Mass);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dCCylinderClass:

        Geom = dCreateCCylinder(0, Radius, Length);
        SetData(&EnvData);

        if(!Static) {
            Body = dBodyCreate(World);
            dMassSetCappedCylinder(&this->Mass, 1, 1, Radius, Length);
            dBodySetMass(Body, &this->Mass);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dCylinderClass:

        Geom = dCreateCylinder(0, Radius, Length);
        SetData(&EnvData);

        if(!Static) {
            Body = dBodyCreate(World);
            dMassSetCylinder(&this->Mass, 1, 1, Radius, Length);
            dBodySetMass(Body, &this->Mass);
            dGeomSetBody(Geom, Body);
        }

        break;

    case dTriMeshClass:

        this->GLModel = Model.GLModel;

        dVector3 *Vertices = new dVector3[Model.Header.NumVertices];
        dTriIndex *Indices = new dTriIndex[Model.Faces.size() * 3];

        for(int i = 0; i != Model.Header.NumVertices; i++) {
            Vertices[i][0] = dReal(Model.Vertices[i].x);
            Vertices[i][1] = dReal(Model.Vertices[i].y);
            Vertices[i][2] = dReal(Model.Vertices[i].z);
        }

        int v = 0;

        for(unsigned int i = 0; i != Model.Faces.size(); i++) {
            Indices[v++] = Model.Faces[i].Vertices[2].Index;
            Indices[v++] = Model.Faces[i].Vertices[1].Index;
            Indices[v++] = Model.Faces[i].Vertices[0].Index;
        }

        dTriMeshDataID Data = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSimple(Data, *Vertices, Model.Header.NumVertices, Indices, Model.Faces.size() * 3);
        Geom = dCreateTriMesh(0, Data, 0, 0, 0);

        if(!Static) {
            Body = dBodyCreate(World);
            dMassSetBox(&this->Mass, 1, 1, 1, 1);
            dBodySetMass(Body, &this->Mass);
            dGeomSetBody(Geom, Body);
            dBodyDisable(Body);

            dBodySetAutoDisableFlag(Body, 1);
            dBodySetAutoDisableDefaults(Body);
        }

        break;

    }

    return true;

}

void CGeneric::SetMass(dReal Mass)
{

    if(Body == 0 || Geom == 0) {
        return;
    }

    dBodyGetMass(Body, &this->Mass);
    dMassAdjust(&this->Mass, Mass);
    dBodySetMass(Body, &this->Mass);

}

void CGeneric::SetData(GeomData *Data)
{
    memcpy(&EnvData, Data, sizeof(GeomData));

}

void CGeneric::SetEnviroment(void)
{
    if(Geom == 0) {
        return;
    }

//    dSpaceAdd(EnvSpace, Geom);
    dSpaceAdd(Space, Geom);

}

void CGeneric::SetPosition(dReal X, dReal Y, dReal Z)
{
    if(Geom == 0) {
        return;
    }

    dGeomSetPosition(Geom, X, Y, Z);
}

void CGeneric::SetRotation(dReal X, dReal Y, dReal Z)
{
//    MTM: theta, phi, psi
//    ODE: phi, theta, psi

    if(Geom == 0) {
        return;
    }

    double Model[16];

    glPushMatrix();
    glRotated(270 - glm::degrees(X), 0, 1, 0);
    glRotated(glm::degrees(-Z), 1, 0, 0);
    glRotated(glm::degrees(Y), 0, 0, 1);
    glGetDoublev(GL_MODELVIEW_MATRIX, Model);
    glPopMatrix();

    const dReal *Position = dGeomGetPosition(Geom);

    dMatrix3 R;
    dRSetIdentity(R);
    OGLtoODE(Model, Position, R);
    dGeomSetRotation(Geom, R);

}


