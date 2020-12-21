#include "results.h"

Results::Results()
{
    maxVelocity = 0.0;
    minVelocity = 0.0;

    velocityCalculated = pressureCalculated = false;
}

Results::Results(vector<Cell> cg, int rows, int cols)
{
    CellGrid = cg;
    results = Mat::zeros(rows, cols, 16);
    pressureField = Mat::zeros(rows, cols, 16);
    qResults = QImage(cols, rows, QImage::Format_RGB32);
    qPressureField = QImage(cols, rows, QImage::Format_RGB32);

    maxVelocity = 0.0;
    minVelocity = 0.0;

    velocityCalculated = pressureCalculated = false;
}

QImage Results::ShowVelocityContour()
{
    if( velocityCalculated )
        return qResults;

    double min = FindMinimumVelocity();
    double max = FindMaximumVelocity();
    double x = 0.0;

    for (int i = 0; i < results.rows; i++)
    {
        for (int j = 0; j < results.cols; j++)
        {
            if (!CellGrid[i * results.cols + j].GetFluid())
            {
                Vec3b v;
                v[0] = 0;
                v[1] = 0;
                v[2] = 0;
                results.at<Vec3b>(i, j) = v;

                QColor color(0, 0, 0);
                qResults.setPixelColor(j, i, color);
            }
            else
            {
                x = ( CellGrid[i * results.cols + j].GetVelocity() - min) / (max - min);
                Vec3b v = SetColour(x);
                results.at<Vec3b>(i, j) = v;

                QColor color(v[2], v[1], v[0]);
                qResults.setPixelColor(j, i, color);
            }
        }
    }

    velocityCalculated = true;

    return qResults;
}

QImage Results::ShowPressureContour()
{
    if( pressureCalculated )
        return qPressureField;

    double min = FindMinimumPressure();
    double max = FindMaximumPressure();
    double x = 0.0;

    for (int i = 0; i < pressureField.rows; i++)
    {
        for (int j = 0; j < pressureField.cols; j++)
        {
            if (!CellGrid[i * pressureField.cols + j].GetFluid())
            {
                Vec3b v;
                v[0] = 0;
                v[1] = 0;
                v[2] = 0;
                pressureField.at<Vec3b>(i, j) = v;

                QColor color(0, 0, 0);
                qPressureField.setPixelColor(j, i, color);
            }
            else
            {
                x = ( CellGrid[i * pressureField.cols + j].GetPressure() - min) / (max - min);
                Vec3b v = SetColour(x);
                pressureField.at<Vec3b>(i, j) = v;

                QColor color(v[2], v[1], v[0]);
                qPressureField.setPixelColor(j, i, color);
            }
        }
    }

    pressureCalculated = true;

    return qPressureField;
}

QImage Results::ShowScale()
{
    QImage scale(SCALE_WIDTH, SCALE_HEIGHT, QImage::Format_RGB32);

    for( int i = 0; i < SCALE_HEIGHT; i++ )
    {
        Vec3b v = SetColour(((double)SCALE_HEIGHT - (double)i)/(double)SCALE_HEIGHT);
        QColor color(v[2], v[1], v[0]);

        for( int j = 0; j < SCALE_WIDTH; j++ )
            scale.setPixelColor(j, i, color);
    }

    return scale;
}

double Results::FindMinimumVelocity()
{
    double min = DBL_MAX;
    for (int i = 0; i < CellGrid.size(); i++)
    {
        if( CellGrid[i].GetFluid() && CellGrid[i].GetVelocity() < min )
            min = CellGrid[i].GetVelocity();
    }

    this->minVelocity = min;

    return min;
}

double Results::FindMaximumVelocity()
{
    double max = DBL_MIN;
    for (int i = 0; i < CellGrid.size(); i++)
    {
        if (CellGrid[i].GetFluid() && CellGrid[i].GetVelocity() > max)
            max = CellGrid[i].GetVelocity();
    }

    this->maxVelocity = max;

    return max;
}

double Results::FindMinimumPressure()
{
    double min = DBL_MAX;
    for (int i = 0; i < CellGrid.size(); i++)
    {
        if( CellGrid[i].GetFluid() && CellGrid[i].GetPressure() < min )
            min = CellGrid[i].GetPressure();
    }

    this->minPressure = min;

    return min;
}

double Results::FindMaximumPressure()
{
    double max = DBL_MIN;
    for (int i = 0; i < CellGrid.size(); i++)
    {
        if (CellGrid[i].GetFluid() && CellGrid[i].GetPressure() > max)
            max = CellGrid[i].GetPressure();
    }

    this->maxPressure = max;

    return max;
}

Vec3b Results::SetColour(double x)
{
    Vec3b v;
    if (x <= 0.5)
    {
        v[0] = floor(255 * (0.5 - x) / 0.5);
        v[1] = floor(255 * x / 0.5);
        v[2] = 0;
    }
    else
    {
        v[0] = 0;
        v[1] = floor(255 * (1.0 - x) / 0.5);
        v[2] = floor(255 * (x - 0.5) / 0.5);
    }
    return v;
}

double Results::GetMinVelocityValue()
{
    return this->minVelocity;
}

double Results::GetMaxVelocityValue()
{
    return this->maxVelocity;
}

double Results::GetMinPressureValue()
{
    return this->minPressure;
}

double Results::GetMaxPressureValue()
{
    return this->maxPressure;
}

void Results::ShowVelocityMat()
{
    namedWindow("Velocity");
    imshow("Velocity", this->results);
    waitKey();
}

void Results::ShowPressureMat()
{
    namedWindow("Pressure");
    imshow("Pressure", this->pressureField);
    waitKey();
}
