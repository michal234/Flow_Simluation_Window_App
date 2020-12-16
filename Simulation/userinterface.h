#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "binarymap.h"

class UserInterface
{
public:
    UserInterface();
    void SetBinaryMap(QString path);

private:
    BinaryMap bm;
};

#endif // USERINTERFACE_H
