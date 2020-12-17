#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "binarymap.h"
#include "solver.h"

class UserInterface
{
public:
    UserInterface();

    void SetBinaryMap(QString path);
    QBitmap GetImage();
    bool BinaryMapIsEmpty();

    void SetSolver();

private:
    BinaryMap bm;
    Solver solver;
};

#endif // USERINTERFACE_H
