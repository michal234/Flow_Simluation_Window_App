#include "userinterface.h"
#include <iostream>

using namespace std;

UserInterface::UserInterface()
{

}

void UserInterface::SetBinaryMap(QString path)
{
    this->bm = BinaryMap(path);
}
