#ifndef RESULTS_H
#define RESULTS_H

#include "cell.h"
#include <QImage>
#include <QColor>

#define SCALE_WIDTH 50
#define SCALE_HEIGHT 1000

using namespace std;
using namespace cv;

class Results
{
    private:
        vector<Cell> CellGrid;
        Mat results;
        QImage qResults;

        double maxVelocity;
        double minVelocity;

        double FindMinimumVelocity();
        double FindMaximumVelocity();
        Vec3b SetColour(double x);

    public:
        Results();
        Results(vector<Cell> cg, int rows, int cols);
        QImage ShowVelocityContour();
        QImage ShowScale();
        double GetMinVelocityValue();
        double GetMaxVelocityValue();
};

#endif // RESULTS_H
