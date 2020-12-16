#include "binarymap.h"

BinaryMap::BinaryMap()  //default constructor
{

}

BinaryMap::BinaryMap(QString path)  //constructor with path to the image
{
    this->input = QBitmap(path);
}

bool BinaryMap::IsEmpty()   //if the input is null returns true
{
    return this->input.isNull();
}


int BinaryMap::GetRows()    //returns number of rows
{
    return this->input.height();
}

int BinaryMap::GetCols()    //return number of columns
{
    return this->input.width();
}

int BinaryMap::GetElement(int i, int j) //return the value of specified pixel
{
    return this->input.toImage().pixelColor(i, j).value();
}
