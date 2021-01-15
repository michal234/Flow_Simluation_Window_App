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
    this->results = Results(this->solver.Simulate(), this->bm.GetRows(), this->bm.GetCols());
}

bool UserInterface::SimulationIsCalculated()
{
    return this->solver.GetCalculated();
}

QImage UserInterface::ShowResults(int variable)
{
    switch(variable)
    {
    case 1:
        return this->results.ShowVelocityContour();

    case 2:
        return this->results.ShowPressureContour();

    case 3:
        return this->results.ShowVelocityXContour();

    case 4:
        return this->results.ShowVelocityYContour();
    }

}

QImage UserInterface::ShowScale()
{
    return this->results.ShowScale();
}

double UserInterface::GetMinValue(int variable)
{
    switch(variable)
    {
    case 1:
        return this->results.GetMinVelocityValue();

    case 2:
        return this->results.GetMinPressureValue();

    case 3:
        return this->results.GetMinVelocityXValue();

    case 4:
        return this->results.GetMinVelocityYValue();
    }
}

double UserInterface::GetMaxValue(int variable)
{
    switch(variable)
    {
    case 1:
        return this->results.GetMaxVelocityValue();

    case 2:
        return this->results.GetMaxPressureValue();

    case 3:
        return this->results.GetMaxVelocityXValue();

    case 4:
        return this->results.GetMaxVelocityYValue();
    }
}
