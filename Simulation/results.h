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
        QImage qResults;
        QImage qPressureField;

        bool velocityCalculated;
        bool pressureCalculated;

        double maxVelocity;
        double minVelocity;

        double maxPressure;
        double minPressure;

        double FindMinimumVelocity();
        double FindMaximumVelocity();

        double FindMinimumPressure();
        double FindMaximumPressure();

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

        QImage ShowScale();
};

#endif // RESULTS_H
