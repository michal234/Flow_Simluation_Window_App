#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "binarymap.h"
#include "solver.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

class UserInterface
{
public:
    UserInterface();

    void SetBinaryMap(QString qPath, const string mPath);
    QBitmap GetImage();
    bool BinaryMapIsEmpty();

    void SetSolver();
    void SetBoundaryConditions(double v, int direction);
    void SetMaxIter(int iter);
    void StartSimulation();

private:
    BinaryMap bm;
    Solver solver;
};

#endif // USERINTERFACE_H
