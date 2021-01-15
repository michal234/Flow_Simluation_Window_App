#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include "binarymap.h"
#include "cell.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define PI 3.14159265

class Solver
{
    private:
        bool initialized;
        bool boundaryConditions;
        bool calculated;

        int cellGridRows;
        int cellGridCols;

        vector<Cell> CellGrid;
        vector<Cell*> FluidCells;

        double v_start;
        double fluid_input;

        int maxIter;
        int start_direction;

        void SetNeighbours();
        int TypeOfNeighbourhood(Cell top, Cell right, Cell bottom, Cell left);

        void UpdateGrid();

        void RemoveExtremalPoints();

    public:
        Solver();

        bool GetInitialized();
        bool GetBoudaryConditions();
        bool GetCalculated();

        void CellGridInitialization(BinaryMap bm);
        void SetBoundaryConditions(double v, int direction);
        void SetMaxIter(int iter);
        vector<Cell> Simulate();
};
#endif // SOLVER_H
