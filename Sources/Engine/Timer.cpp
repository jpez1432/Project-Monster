
#include "Timer.hpp"

using namespace Engine;

CTimer::CTimer(void)
{
    LastTime = CurrentTime = glfwGetTime();
    DeltaTime = Counter = LastFrame = 0;
}

void CTimer::Reset(void)
{
    LastTime = CurrentTime = glfwGetTime();
    DeltaTime = Counter = LastFrame = 0;
}

const double CTimer::Update(void)
{
    Counter++;
    CurrentTime = glfwGetTime();

    if((CurrentTime - LastTime) > 1.0f) {
        Fps = Counter;
        Counter = 0;
        LastTime++;
    }

    DeltaTime = CurrentTime - LastFrame;
    LastFrame = CurrentTime;

//    if (Config->General.Paused) {
//        DeltaTime = 0.0f;
//    }

    return DeltaTime;
}
