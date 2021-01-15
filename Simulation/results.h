#ifndef RESULTS_H
#define RESULTS_H

#include "cell.h"
#include <QImage>
#include <QColor>

#define SCALE_WIDTH 100
#define SCALE_HEIGHT 1000

using namespace std;
using namespace cv;

class Results
{
    private:
        vector<Cell> CellGrid;
        Mat results;
        Mat pressureField;
        Mat velocityX;
        Mat velocityY;
        QImage qResults;
        QImage qPressureField;
        QImage qVelocityX;
        QImage qVelocityY;

        bool velocityCalculated;
        bool pressureCalculated;
        bool velocityXCalculated;
        bool velocityYCalculated;

        double maxVelocity;
        double minVelocity;

        double maxPressure;
        double minPressure;

        double maxVelocityX;
        double minVelocityX;

        double maxVelocityY;
        double minVelocityY;

        double FindMinimumVelocity();
        double FindMaximumVelocity();

        double FindMinimumPressure();
        double FindMaximumPressure();

        double FindMinimumVelocityX();
        double FindMaximumVelocityX();

        double FindMinimumVelocityY();
        double FindMaximumVelocityY();

        Vec3b SetColour(double x);

    public:
        Results();
        Results(vector<Cell> cg, int rows, int cols);

        QImage ShowVelocityContour();
        double GetMinVelocityValue();
        double GetMaxVelocityValue();

        QImage ShowPressureContour();
        double GetMinPressureValue();
        double GetMaxPressureValue();

        QImage ShowVelocityXContour();
        double GetMinVelocityXValue();
        double GetMaxVelocityXValue();

        QImage ShowVelocityYContour();
        double GetMinVelocityYValue();
        double GetMaxVelocityYValue();

        QImage ShowScale();

};

#endif // RESULTS_H
