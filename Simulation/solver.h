#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include "binarymap.h"
#include "cell.h"

#define PI 3.14159265

class Solver
{
    private:
        bool initialized;
        bool boundaryConditions;

        int cellGridRows;
        int cellGridCols;

        vector<Cell> CellGrid;
        vector<Cell*> FluidCells;

        double v_start;
        double fluid_input;



        void SetNeighbours();
        int TypeOfNeighbourhood(Cell top, Cell right, Cell bottom, Cell left);

        void UpdateGrid();

        void ShowStep();
        void Standarization();
        void RemoveExtremalPoints();

    public:
        Solver();

        bool GetInitialized();
        bool GetBoudaryConditions();

        void CellGridInitialization(BinaryMap bm);
        void SetBoundaryConditions(double v);
        vector<Cell> Simulate();
};
#endif // SOLVER_H
