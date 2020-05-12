
#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include <GL/Glew.h>
#include <GLFW/Glfw3.h>
#include <glm.hpp>

namespace Engine
{

class CFrustum
{

private:

    glm::vec4 Planes[6];

    enum PlaneFaces { Right = 0, Left, Bottom, Top, Back, Front };

public:

    CFrustum(void);
    ~CFrustum(void);

    void Update(void);
    void UpdateFast(void);

    enum Results { Outside = 0, Intersects, Inside };

    Results BoxInFrustum(glm::vec3 Min, glm::vec3 Max);
    Results SphereInFrustum(glm::vec3 Position, float Size);
    Results PointInFrustum(glm::vec3 Point);
};

}


#endif
