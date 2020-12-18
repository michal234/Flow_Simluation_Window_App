#include "userinterface.h"
#include <iostream>

using namespace std;

UserInterface::UserInterface()
{

}

void UserInterface::SetBinaryMap(QString qPath, const string mPath)
{
    this->bm = BinaryMap(qPath, mPath);
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

void UserInterface::SetBoundaryConditions(double v, int direction)
{
    this->solver.SetBoundaryConditions(v, direction);
}

void UserInterface::SetMaxIter(int iter)
{
    this->solver.SetMaxIter(iter);
}

void UserInterface::StartSimulation()
{
    this->solver.Simulate();
}
