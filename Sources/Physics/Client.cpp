
#include "Client.hpp"

using namespace Physics;

CClient::CClient(void)
{

}

CClient::~CClient(void)
{

}

bool CClient::Initialize(dWorldID World, dSpaceID Space, dSpaceID EnvSpace)
{
    if(World == 0 || Space == 0 || EnvSpace == 0) {
        return false;
    }

    this->World = World;
    this->Space = Space;
    this->EnvSpace = EnvSpace;

    return true;

}

bool CClient::Update(dReal Step)
{

    return true;
}

void CClient::Render(void)
{


}
