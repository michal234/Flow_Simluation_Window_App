#ifndef BINARYMAP_H
#define BINARYMAP_H

#include <QString>
#include <QBitmap>

class BinaryMap
{
public:
    BinaryMap();
    BinaryMap(QString path);
    bool IsEmpty();
    int GetRows();
    int GetCols();
    int GetElement(int i, int j);
    QBitmap GetImage();

private:
    QBitmap input;
};

#endif // BINARYMAP_H
