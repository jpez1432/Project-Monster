
#ifndef INPUT_HPP
#define INPUT_HPP

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Engine
{

class CInput
{

private:

    int AxesCount, ButtonCount;

    const float *Axis;
    const unsigned char *Buttons;

public:

    CInput(void);
    ~CInput(void);

    bool Create(int Index);
    void Update(void);

    bool JoystickPresent(void)
    {
        return glfwJoystickPresent(GLFW_JOYSTICK_1);
    }

    const unsigned char GetButtonState(int Num);
    const float GetAxisState(int Num);

};
}

#endif
