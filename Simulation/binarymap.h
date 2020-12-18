#ifndef BINARYMAP_H
#define BINARYMAP_H

#include <QString>
#include <QBitmap>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class BinaryMap
{
public:
    BinaryMap();
    BinaryMap(QString qPath, const string mPath);
    bool IsEmpty();
    int GetRows();
    int GetCols();
    int GetElement(int i, int j);
    QBitmap GetImage();

private:
    QBitmap qImage;
    Mat mImage;

    Mat ConvertToOneChannel(Mat src);
};

#endif // BINARYMAP_H
