
#include "Camera.hpp"

using namespace Engine;

CCamera::CCamera(void) : Yaw(0.0f), Pitch(0.0f), Angle(0.0f), Speed(0.0f)
{
    int Width, Height;
    glfwGetFramebufferSize(GLWindow, &Width, &Height);

    HalfWidth = LastX = Width >> 2;
    HalfHeight = LastY = Height >> 2;
}

glm::mat4x4 CCamera::Update(const double Delta, Physics::CVehicle *Vehicle)
{
    if(Config->General.Paused) {
        return glm::lookAt(Position, Center, Up);
    }

    static glm::vec3 Forward, Right, Direction;

    Centrum = Vehicle->GetPosition();
    glm::vec3 CenterFront = Vehicle->GetWheelPosition(0, 1) + ((Vehicle->GetWheelPosition(0, 0) - Vehicle->GetWheelPosition(0, 1)) * 0.5f);
    glm::vec3 CenterBack = Vehicle->GetWheelPosition(1, 1) + ((Vehicle->GetWheelPosition(1, 0) - Vehicle->GetWheelPosition(1, 1)) * 0.5f);

    Angle = glm::degrees(atan2f(CenterBack.z - CenterFront.z, CenterBack.x - CenterFront.x));
    Angle -= 360.0f * std::floor(Angle * (1.0f / 360.0f));

    Speed = Config->Input.MovementSpeed * Delta;

    if(glfwGetKey(GLWindow, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(GLWindow, GLFW_KEY_RIGHT_SHIFT)) {
        Speed *= 10.0f;
    }

    switch(Config->Camera.Mode) {

    case 0:

        Forward = glm::normalize(CenterFront - CenterBack);
        Right = glm::normalize(glm::cross(Forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        Up = glm::normalize(glm::cross(Right, Forward));

        Position = Centrum;
        Center = Position + Forward;

        Position.y += 0.2f;
        Center.y += 0.2f;

        break;

    case 1:

        Direction = CenterBack - CenterFront;
        Position = CenterBack + (Direction * float ((Config->Camera.Radius)));
        Center = Centrum;
        Up = glm::vec3(0, 1, 0);

        Position.y = Centrum.y + Config->Camera.Radius / 8.0f;

        break;

    case 2:

        Center = Centrum;
        Up = glm::vec3(0, 1, 0);

        if((rand() % 5 + 1) == 1) {
            if(glm::distance(Position, Center) >= 10.0f) {
                srand(time(NULL));
                Position = Center + glm::vec3(rand() % 5 + 1, rand() % 2 + 1, rand() % 5 + 1);
            }
        }

        break;

    case 3:

        if(Config->Input.MouseLook) {

            glfwGetCursorPos(GLWindow, &XPos, &YPos);
            glfwSetCursorPos(GLWindow, HalfWidth, HalfHeight);

            XOffset = (XPos - HalfWidth) * Config->Input.MouseSensitivity;
            YOffset = (YPos - HalfHeight) * Config->Input.MouseSensitivity;

            if(Config->Camera.Invert) {
                YOffset *= -1;
            }

            Yaw += XOffset;
            Pitch += YOffset;

            Yaw -= 360.0f * std::floor(Yaw * (1.0f / 360.0f));
            Pitch -= 360.0f * std::floor(Pitch * (1.0f / 360.0f));

            Center.x = glm::cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Center.y = glm::sin(glm::radians(Pitch));
            Center.z = glm::sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        }

        if(glfwGetKey(GLWindow, Config->Input.Forwards)) {
            Position += Speed * Center;
        }


        if(glfwGetKey(GLWindow, Config->Input.Backwards)) {
            Position -= Speed * Center;
        }


        if(glfwGetKey(GLWindow, Config->Input.StrafeLeft)) {
            Position -= Speed * glm::normalize(glm::cross(Center, Up));
        }


        if(glfwGetKey(GLWindow, Config->Input.StrafeRight)) {
            Position += Speed * glm::normalize(glm::cross(Center, Up));
        }

        if(glfwGetKey(GLWindow, Config->Input.Up)) {
            Position.y += Speed;
        }

        if(glfwGetKey(GLWindow, Config->Input.Down)) {
            Position.y -= Speed;
        }

        Center += Position;

        break;

    case 4:

        if(Config->Input.MouseLook) {

            glfwGetCursorPos(GLWindow, &XPos, &YPos);
            glfwSetCursorPos(GLWindow, HalfWidth, HalfHeight);

            XOffset = (XPos - HalfWidth) * Config->Input.MouseSensitivity;
            YOffset = (YPos - HalfHeight) * Config->Input.MouseSensitivity;

            if(Config->Camera.Invert) {
                YOffset *= -1;
            }

            Yaw += XOffset;
            Pitch += YOffset;

            Yaw -= 360.0f * std::floor(Yaw * (1.0f / 360.0f));
            Pitch -= 360.0f * std::floor(Pitch * (1.0f / 360.0f));

            Center = Centrum;
            Position.y = (Center.y + (float)(Config->Camera.Radius / 2) * sin(glm::radians(Pitch)));
            Position.x = (Center.x + (float)(Config->Camera.Radius / 2) * cos(glm::radians(Pitch)) * cos(glm::radians(Yaw)));
            Position.z = (Center.z + (float)(Config->Camera.Radius / 2) * cos(glm::radians(Pitch)) * sin(glm::radians(Yaw)));

            Up = glm::vec3(0.0f, 1.0f, 0.0f);

        }

        break;

    }

//    if(Position.y < Centrum.y) {
//        Position.y = Centrum.y;
//    }

    return glm::lookAt(Position, Center, Up);
}
