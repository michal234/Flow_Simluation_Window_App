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

QBitmap UserInterface::GetImage()
{
    return this->bm.GetImage();
}

bool UserInterface::BinaryMapIsEmpty()
{
    return this->bm.IsEmpty();
}

void UserInterface::SetSolver()
{
    this->solver = Solver();
    if( !this->bm.IsEmpty() )
        this->solver.CellGridInitialization(this->bm);
}
