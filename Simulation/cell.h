#ifndef CELL_H
#define CELL_H

#include <vector>
#include <map>
#include <limits>
#include <cmath>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#define PI 3.14159265
//#define MAX_VALUE 50
#define DENSITY 1000

using namespace std;

class Cell
{
    private:
        bool fluid;		//if cell is fluid it's true
        bool source;	//if cell is a boundary condition it is true
        bool boundary;	//if cell lays on X=0 or X=N-1 or Y=0 or Y=M-1 it's true
        bool outlet;
        bool outer_boundary;

        int x;	//X-coordinate of the cell
        int y;	//Y-coordinate of the cell

        map<string, Cell*> neighbours;	//neighbours of the cell
        map<string, Cell*> neighboursOnSlant;
        int typeOfNeighbourhood;
        int typeOfNeighbourhoodOnSlant;

        double fluid_amount;	//amount of fluid in the cell (= total_input)
        double velocity;
        double startVelocity;
        double prevVelocity;

        //amount of fluid that flows into the cell in current step
        double input_top;
        double input_right;
        double input_bottom;
        double input_left;

        //amount of fluid that flows into the cell in the next step
        double input_top_next;
        double input_right_next;
        double input_bottom_next;
        double input_left_next;

        //total amount of fluid that flows into the cell in the current step
        double input_total;

        //amount of fluid that flows from the cell to the neighbouring cells in the current step
        double output_top;
        double output_right;
        double output_bottom;
        double output_left;

        //amount of fluid that flows from the cell to the neighbouring cells in the next step
        double output_top_next;
        double output_right_next;
        double output_bottom_next;
        double output_left_next;

        //total amount of fluid that flows from the cell to the neighbours in the current step
        double output_total;
        double output_total_next;

        void UniformFlow();
        void FlowToNeighbours(double top_flow, double right_flow, double bottom_flow, double left_flow);
        void FlowToNeighboursOnSlant(double top_flow, double right_flow, double bottom_flow, double left_flow, int direction);
        int ChooseDirection(double x_direction, double y_direction);
        int ChooseDirection2(int ox, int oy, int quadrant, double angle);

    public:
        Cell();
        Cell(bool fluid, bool outer);	//for usage of setting neighbours in solver
        Cell(bool fluid, bool boundary, int x, int y);

        bool GetFluid();
        bool GetBoundary();
        bool GetSource();
        bool GetBalance();
        bool GetOutlet();
        bool GetOuterBoundary();

        double GetFluidAmount();
        double GetVelocity();
        double GetPressure();

        int GetX();
        int GetY();

        void SetSource();
        void SetOutlet();

        //void SetNeighbours(map<string, Cell*> neighbours);
        void SetNeighbours(Cell* top, Cell* right, Cell* bottom, Cell* left);
        void SetTypeOfNeighbourhood(int type);

        void SetNeighboursOnSlant(Cell* topRight, Cell* bottomRight, Cell* bottomLeft, Cell* topLeft);
        void SetTypeOfNeighbourhoodOnSlant(int type);

        void FluidFlow();

        void SetTopInput(double input);
        void SetRightInput(double input);
        void SetBottomInput(double input);
        void SetLeftInput(double input);

        void SetStartVelocity(double v);

        void Update();
        void StandarizeCell(double factor);

        void GetMeanFromNeighbours(double sigma);
};

#endif // CELL_H
