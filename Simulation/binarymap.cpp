#include "binarymap.h"

using namespace std;
using namespace cv;

BinaryMap::BinaryMap()  //default constructor
{

}

BinaryMap::BinaryMap(QString qPath, const string mPath)  //constructor with path to the image
{
    this->qImage = QBitmap(qPath);

    Mat img = imread(mPath);
    if( img.channels() > 1 )
        this->mImage = ConvertToOneChannel(img);
    else
        this->mImage = img;
}

bool BinaryMap::IsEmpty()   //if the input is null returns true
{
    return this->mImage.empty();
}


int BinaryMap::GetRows()    //returns number of rows
{
    return this->mImage.rows;
}

int BinaryMap::GetCols()    //return number of columns
{
    return this->mImage.cols;
}

int BinaryMap::GetElement(int i, int j) //return the value of specified pixel
{
    return this->mImage.at<uchar>(i, j);
}

Mat BinaryMap::ConvertToOneChannel(Mat src)
{
    Mat res = Mat::zeros(src.rows, src.cols, 0);
    for( int i = 0; i < src.rows; i++ )
        for (int j = 0; j < src.cols; j++)
            res.at<uchar>(i, j) = src.at<Vec3b>(i, j)[0];

    return res;
}

QBitmap BinaryMap::GetImage()
{
    return this->qImage;
}
