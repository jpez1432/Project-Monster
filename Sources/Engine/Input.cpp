
#include "Input.hpp"

using namespace Engine;

CInput::CInput(void)
{
    Update();
}

CInput::~CInput(void)
{

}
bool CInput::Create(int Index)
{
    if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_FALSE) {
        return false;
    } else {
        return true;
    }
}



void CInput::Update(void)
{
    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        Axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &AxesCount);
        Buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &ButtonCount);
    }
}

const unsigned char CInput::GetButtonState(int Num)
{
    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        return Buttons[Num];
    } else {
        return 0;
    }
}

const float CInput::GetAxisState(int Num)
{
    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        return Axis[Num];
    } else {
        return 0;
    }
}

