

#include "Frustum.hpp"

using namespace Engine;

CFrustum::CFrustum(void)
{
}

CFrustum::~CFrustum(void)
{
}

void CFrustum::UpdateFast(void)
{
    GLfloat   Clip[16];
    GLfloat   Proj[16];
    GLfloat   Modl[16];

    glGetFloatv(GL_PROJECTION_MATRIX, Proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, Modl);

    Clip[ 0] = Modl[ 0] * Proj[ 0];
    Clip[ 1] = Modl[ 1] * Proj[ 5];
    Clip[ 2] = Modl[ 2] * Proj[10] + Modl[ 3] * Proj[14];
    Clip[ 3] = Modl[ 2] * Proj[11];

    Clip[ 4] = Modl[ 4] * Proj[ 0];
    Clip[ 5] = Modl[ 5] * Proj[ 5];
    Clip[ 6] = Modl[ 6] * Proj[10] + Modl[ 7] * Proj[14];
    Clip[ 7] = Modl[ 6] * Proj[11];

    Clip[ 8] = Modl[ 8] * Proj[ 0];
    Clip[ 9] = Modl[ 9] * Proj[ 5];
    Clip[10] = Modl[10] * Proj[10] + Modl[11] * Proj[14];
    Clip[11] = Modl[10] * Proj[11];

    Clip[12] = Modl[12] * Proj[ 0];
    Clip[13] = Modl[13] * Proj[ 5];
    Clip[14] = Modl[14] * Proj[10] + Modl[15] * Proj[14];
    Clip[15] = Modl[14] * Proj[11];

    Planes[Right].x = Clip[ 3] - Clip[ 0];
    Planes[Right].y = Clip[ 7] - Clip[ 4];
    Planes[Right].z = Clip[11] - Clip[ 8];
    Planes[Right].w = Clip[15] - Clip[12];
    Planes[Right] = glm::normalize(Planes[Right]);

    Planes[Left].x = Clip[ 3] + Clip[ 0];
    Planes[Left].y = Clip[ 7] + Clip[ 4];
    Planes[Left].z = Clip[11] + Clip[ 8];
    Planes[Left].w = Clip[15] + Clip[12];
    Planes[Left] = glm::normalize(Planes[Left]);

    Planes[Bottom].x = Clip[ 3] + Clip[ 1];
    Planes[Bottom].y = Clip[ 7] + Clip[ 5];
    Planes[Bottom].z = Clip[11] + Clip[ 9];
    Planes[Bottom].w = Clip[15] + Clip[13];
    Planes[Bottom] = glm::normalize(Planes[Bottom]);

    Planes[Top].x = Clip[ 3] - Clip[ 1];
    Planes[Top].y = Clip[ 7] - Clip[ 5];
    Planes[Top].z = Clip[11] - Clip[ 9];
    Planes[Top].w = Clip[15] - Clip[13];
    Planes[Top] = glm::normalize(Planes[Top]);

    Planes[Back].x = Clip[ 3] - Clip[ 2];
    Planes[Back].y = Clip[ 7] - Clip[ 6];
    Planes[Back].z = Clip[11] - Clip[10];
    Planes[Back].w = Clip[15] - Clip[14];
    Planes[Back] = glm::normalize(Planes[Back]);

    Planes[Front].x = Clip[ 3] + Clip[ 2];
    Planes[Front].y = Clip[ 7] + Clip[ 6];
    Planes[Front].z = Clip[11] + Clip[10];
    Planes[Front].w = Clip[15] + Clip[14];
    Planes[Front] = glm::normalize(Planes[Front]);

}

void CFrustum::Update(void)
{
    GLfloat   Proj[16];
    GLfloat   Modl[16];
    GLfloat   Clip[16];

    glGetFloatv(GL_PROJECTION_MATRIX, Proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, Modl);

    Clip[ 0] = Modl[ 0] * Proj[ 0] + Modl[ 1] * Proj[ 4] + Modl[ 2] * Proj[ 8] + Modl[ 3] * Proj[12];
    Clip[ 1] = Modl[ 0] * Proj[ 1] + Modl[ 1] * Proj[ 5] + Modl[ 2] * Proj[ 9] + Modl[ 3] * Proj[13];
    Clip[ 2] = Modl[ 0] * Proj[ 2] + Modl[ 1] * Proj[ 6] + Modl[ 2] * Proj[10] + Modl[ 3] * Proj[14];
    Clip[ 3] = Modl[ 0] * Proj[ 3] + Modl[ 1] * Proj[ 7] + Modl[ 2] * Proj[11] + Modl[ 3] * Proj[15];

    Clip[ 4] = Modl[ 4] * Proj[ 0] + Modl[ 5] * Proj[ 4] + Modl[ 6] * Proj[ 8] + Modl[ 7] * Proj[12];
    Clip[ 5] = Modl[ 4] * Proj[ 1] + Modl[ 5] * Proj[ 5] + Modl[ 6] * Proj[ 9] + Modl[ 7] * Proj[13];
    Clip[ 6] = Modl[ 4] * Proj[ 2] + Modl[ 5] * Proj[ 6] + Modl[ 6] * Proj[10] + Modl[ 7] * Proj[14];
    Clip[ 7] = Modl[ 4] * Proj[ 3] + Modl[ 5] * Proj[ 7] + Modl[ 6] * Proj[11] + Modl[ 7] * Proj[15];

    Clip[ 8] = Modl[ 8] * Proj[ 0] + Modl[ 9] * Proj[ 4] + Modl[10] * Proj[ 8] + Modl[11] * Proj[12];
    Clip[ 9] = Modl[ 8] * Proj[ 1] + Modl[ 9] * Proj[ 5] + Modl[10] * Proj[ 9] + Modl[11] * Proj[13];
    Clip[10] = Modl[ 8] * Proj[ 2] + Modl[ 9] * Proj[ 6] + Modl[10] * Proj[10] + Modl[11] * Proj[14];
    Clip[11] = Modl[ 8] * Proj[ 3] + Modl[ 9] * Proj[ 7] + Modl[10] * Proj[11] + Modl[11] * Proj[15];

    Clip[12] = Modl[12] * Proj[ 0] + Modl[13] * Proj[ 4] + Modl[14] * Proj[ 8] + Modl[15] * Proj[12];
    Clip[13] = Modl[12] * Proj[ 1] + Modl[13] * Proj[ 5] + Modl[14] * Proj[ 9] + Modl[15] * Proj[13];
    Clip[14] = Modl[12] * Proj[ 2] + Modl[13] * Proj[ 6] + Modl[14] * Proj[10] + Modl[15] * Proj[14];
    Clip[15] = Modl[12] * Proj[ 3] + Modl[13] * Proj[ 7] + Modl[14] * Proj[11] + Modl[15] * Proj[15];

    Planes[Right].x = Clip[ 3] - Clip[ 0];
    Planes[Right].y = Clip[ 7] - Clip[ 4];
    Planes[Right].z = Clip[11] - Clip[ 8];
    Planes[Right].w = Clip[15] - Clip[12];
    Planes[Right] = glm::normalize(Planes[Right]);

    Planes[Left].x = Clip[ 3] + Clip[ 0];
    Planes[Left].y = Clip[ 7] + Clip[ 4];
    Planes[Left].z = Clip[11] + Clip[ 8];
    Planes[Left].w = Clip[15] + Clip[12];
    Planes[Left] = glm::normalize(Planes[Left]);

    Planes[Bottom].x = Clip[ 3] + Clip[ 1];
    Planes[Bottom].y = Clip[ 7] + Clip[ 5];
    Planes[Bottom].z = Clip[11] + Clip[ 9];
    Planes[Bottom].w = Clip[15] + Clip[13];
    Planes[Bottom] = glm::normalize(Planes[Bottom]);

    Planes[Top].x = Clip[ 3] - Clip[ 1];
    Planes[Top].y = Clip[ 7] - Clip[ 5];
    Planes[Top].z = Clip[11] - Clip[ 9];
    Planes[Top].w = Clip[15] - Clip[13];
    Planes[Top] = glm::normalize(Planes[Top]);

    Planes[Back].x = Clip[ 3] - Clip[ 2];
    Planes[Back].y = Clip[ 7] - Clip[ 6];
    Planes[Back].z = Clip[11] - Clip[10];
    Planes[Back].w = Clip[15] - Clip[14];
    Planes[Back] = glm::normalize(Planes[Back]);

    Planes[Front].x = Clip[ 3] + Clip[ 2];
    Planes[Front].y = Clip[ 7] + Clip[ 6];
    Planes[Front].z = Clip[11] + Clip[10];
    Planes[Front].w = Clip[15] + Clip[14];
    Planes[Front] = glm::normalize(Planes[Front]);
}

CFrustum::Results CFrustum::BoxInFrustum(glm::vec3 Min, glm::vec3 Max)
{
    int C, C2 = 0;

    for (int p = 0; p < 6; p++) {

        C = 0;

        if (Planes[p].x * (Min.x) + Planes[p].y * (Min.y) + Planes[p].z * (Min.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Max.x) + Planes[p].y * (Min.y) + Planes[p].z * (Min.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Min.x) + Planes[p].y * (Max.y) + Planes[p].z * (Min.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Max.x) + Planes[p].y * (Max.y) + Planes[p].z * (Min.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Min.x) + Planes[p].y * (Min.y) + Planes[p].z * (Max.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Max.x) + Planes[p].y * (Min.y) + Planes[p].z * (Max.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Min.x) + Planes[p].y * (Max.y) + Planes[p].z * (Max.z) + Planes[p].w > 0) {
            C++;
        }

        if (Planes[p].x * (Max.x) + Planes[p].y * (Max.y) + Planes[p].z * (Max.z) + Planes[p].w > 0) {
            C++;
        }

        if (C == 0) {
            return Outside;
        }

        if (C == 8) {
            C2++;
        }

    }

    return (C2 == 6) ? Inside : Intersects;

}

CFrustum::Results CFrustum::SphereInFrustum(glm::vec3 Position, float Size)
{

    //return Inside;

    float w = 0.0f;

    for (int p = 0; p < 6; p++) {

        w = Planes[p].x * Position.x + Planes[p].y * Position.y + Planes[p].z * Position.z + Planes[p].w;

        if (w < -Size) {
            return Outside;
        } else {
           return Intersects;
        }

    }

    return Inside;

}

CFrustum::Results CFrustum::PointInFrustum(glm::vec3 Position)
{

    for (int p = 0; p < 6; p++) {

        if (Planes[p].x * Position.x + Planes[p].y * Position.y + Planes[p].z * Position.z + Planes[p].w <= 0) {
            return Outside;
        }

    }

    return Inside;

}
