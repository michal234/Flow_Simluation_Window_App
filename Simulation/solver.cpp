#include "solver.h"

using namespace std;

Solver::Solver()
{
    initialized = false;
    boundaryConditions = false;
    calculated = false;
    cellGridCols = 0;
    cellGridRows = 0;
    v_start = 0.0;
    fluid_input = 0.0;
    maxIter = 100;
    start_direction = 0;
}

bool Solver::GetInitialized()
{
    return initialized;
}

bool Solver::GetBoudaryConditions()
{
    return boundaryConditions;
}

bool Solver::GetCalculated()
{
    return calculated;
}

void Solver::CellGridInitialization(BinaryMap bm)
{
    cellGridRows = bm.GetRows();
    cellGridCols = bm.GetCols();
    for (int i = 0; i < cellGridRows; i++)
    {
        for (int j = 0; j < cellGridCols; j++)
        {
            int element = bm.GetElement(i, j);
            bool fluid = true;
            bool boundary = false;

            if (element == 0)	//solid
                fluid = false;
            if( i==0 || i == cellGridRows-1 || j==0 || j==cellGridCols-1 )
                boundary = true;

            Cell cell(fluid, boundary, i, j);

            CellGrid.push_back(cell);

        }
    }

    for (int i = 0; i < cellGridRows; i++)
    {
        for (int j = 0; j < cellGridCols; j++)
        {
            if (CellGrid[i * cellGridCols + j].GetFluid())
            {
                FluidCells.push_back(&CellGrid[i*cellGridCols+j]);
            }
        }
    }

    initialized = true;
}

void Solver::SetBoundaryConditions(double v, int direction)
{
    for (int i = 0; i < FluidCells.size(); i++)
    {
        if (FluidCells[i]->GetFluid())
        {
            FluidCells[i]->SetStartVelocity(v);
            switch(direction)
            {
            case 1:
                FluidCells[i]->SetLeftInput(v);
                if( FluidCells[i]->GetY() == 0 )
                {
                    FluidCells[i]->SetSource();
                    fluid_input += v;
                }
                break;

            case 2:
                FluidCells[i]->SetRightInput(v);
                if( FluidCells[i]->GetY() == cellGridCols - 1 )
                {
                    FluidCells[i]->SetSource();
                    fluid_input += v;
                }
                break;

            case 3:
                FluidCells[i]->SetTopInput(v);
                if( FluidCells[i]->GetX() == 0 )
                {
                    FluidCells[i]->SetSource();
                    fluid_input += v;
                }
                break;

            case 4:
                FluidCells[i]->SetBottomInput(v);
                if( FluidCells[i]->GetX() == cellGridRows - 1 )
                {
                    FluidCells[i]->SetSource();
                    fluid_input += v;
                }
                break;
            }
        }
    }

    boundaryConditions = true;
    v_start = v;
    start_direction = direction;
}

void Solver::SetMaxIter(int iter)
{
    this->maxIter = iter;
}

vector<Cell> Solver::Simulate()
{
    SetNeighbours();
    UpdateGrid();

    int unbalancedCells = 0;
    int x = 1;

    do
    {
        unbalancedCells = 0;
        for (int i = 0; i < FluidCells.size(); i++)
        {
            FluidCells[i]->FluidFlow();
            if( !FluidCells[i]->GetBalance() )
                unbalancedCells++;
        }

        //if( x%10 == 0 )
          //  RemoveExtremalPoints();

        UpdateGrid();
        //ShowStep();
        x++;
    }while(x <= this->maxIter && unbalancedCells != 0);

    //RemoveExtremalPoints();
    //Standarization();
    //UpdateGrid();
    RemoveExtremalPoints();
    //ShowStep();

    calculated = true;

    return CellGrid;
}

void Solver::SetNeighbours()
{
    //cout << "Trwa przydzielanie sasiadow dla kazdej komorki...\n";

    for (int i = 0; i < FluidCells.size(); i++)
    {
        Cell* cell = FluidCells[i];
        int x = cell->GetX();
        int y = cell->GetY();
        //map<string, Cell*> neighbours = map<string, Cell*>();

        Cell outer = Cell(true, true);	//default fluid cell for cells on the boundary

        Cell* top;
        Cell* right;
        Cell* bottom;
        Cell* left;

        Cell* topRight;
        Cell* bottomRight;
        Cell* bottomLeft;
        Cell* topLeft;

        int type = 0;
        int type_slant = 0;


        if (!cell->GetBoundary())	//if the cell lays in the interior
        {
            /*Cell* top = &CellGrid[x-1][y];
            Cell* right = &CellGrid[x][y+1];
            Cell* bottom = &CellGrid[x+1][y];
            Cell* left = &CellGrid[x][y-1];*/

            top = &CellGrid[(x-1)*cellGridCols + y];
            right = &CellGrid[x*cellGridCols + y + 1];
            bottom = &CellGrid[(x+1)*cellGridCols + y];
            left = &CellGrid[x*cellGridCols + y - 1];

            topRight = &CellGrid[(x-1)*cellGridCols + y + 1];
            bottomRight = &CellGrid[(x+1)*cellGridCols + y + 1];
            bottomLeft = &CellGrid[(x+1)*cellGridCols + y - 1];
            topLeft = &CellGrid[(x-1)*cellGridCols + y - 1];

            type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
            type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);

        }
        else	//if the cell lays on the boundary
        {
            if (x == 0)		//top boundary
            {
                if (y == 0)	//top left corner
                {
                    /*Cell* top = &outer;
                    Cell* right = &CellGrid[x][y + 1];
                    Cell* bottom = &CellGrid[x + 1][y];
                    Cell* left = &outer;*/

                    top = &outer;
                    right = &CellGrid[x*cellGridCols + y + 1];
                    bottom = &CellGrid[(x+1)*cellGridCols + y];
                    left = &outer;

                    topRight = &outer;
                    bottomRight = &CellGrid[(x + 1) * cellGridCols + y + 1];
                    bottomLeft = &outer;
                    topLeft = &outer;

                    type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                    type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);

                }
                else if (y == cellGridCols - 1)	//top right corner
                {
                    /*Cell* top = &outer;
                    Cell* right = &outer;
                    Cell* bottom = &CellGrid[x + 1][y];
                    Cell* left = &CellGrid[x][y - 1];*/

                    top = &outer;
                    right = &outer;
                    bottom = &CellGrid[(x+1)*cellGridCols + y];
                    left = &CellGrid[x*cellGridCols + y - 1];

                    topRight = &outer;
                    bottomRight = &outer;
                    bottomLeft = &CellGrid[(x + 1) * cellGridCols + y - 1];
                    topLeft = &outer;

                    type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                    type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
                }
                else	//the rest of the top boundary
                {
                    /*Cell* top = &outer;
                    Cell* right = &CellGrid[x][y + 1];
                    Cell* bottom = &CellGrid[x + 1][y];
                    Cell* left = &CellGrid[x][y - 1];*/

                    top = &outer;
                    right = &CellGrid[x*cellGridCols + y + 1];
                    bottom = &CellGrid[(x+1)*cellGridCols + y];
                    left = &CellGrid[x * cellGridCols + y - 1];

                    topRight = &outer;
                    bottomRight = &CellGrid[(x + 1) * cellGridCols + y + 1];
                    bottomLeft = &CellGrid[(x + 1) * cellGridCols + y - 1];
                    topLeft = &outer;

                    type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                    type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
                }
            }
            else if (x == cellGridRows - 1)	//bottom boundary
            {
                if (y == 0)	//bottom left corner
                {
                    /*Cell* top = &CellGrid[x - 1][y];
                    Cell* right = &CellGrid[x][y + 1];
                    Cell* bottom = &outer;
                    Cell* left = &outer;*/

                    top = &CellGrid[(x-1)*cellGridCols + y];
                    right = &CellGrid[x * cellGridCols + y + 1];
                    bottom = &outer;
                    left = &outer;

                    topRight = &CellGrid[(x - 1) * cellGridCols + y + 1];
                    bottomRight = &outer;
                    bottomLeft = &outer;
                    topLeft = &outer;

                    type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                    type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
                }
                else if (y == cellGridCols - 1)	//bottom right corner
                {
                    /*Cell* top = &CellGrid[x - 1][y];
                    Cell* right = &outer;
                    Cell* bottom = &outer;
                    Cell* left = &CellGrid[x][y - 1];*/

                    top = &CellGrid[(x-1)*cellGridCols + y];
                    right = &outer;
                    bottom = &outer;
                    left = &CellGrid[x*cellGridCols + y - 1];

                    topRight = &outer;
                    bottomRight = &outer;
                    bottomLeft = &outer;
                    topLeft = &CellGrid[(x - 1) * cellGridCols + y - 1];

                    type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                    type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
                }
                else	//the rest of the bottom boundary
                {
                    /*Cell* top = &CellGrid[x - 1][y];
                    Cell* right = &CellGrid[x][y + 1];
                    Cell* bottom = &outer;
                    Cell* left = &CellGrid[x][y - 1];*/

                    top = &CellGrid[(x-1)*cellGridCols + y];
                    right = &CellGrid[x*cellGridCols + y + 1];
                    bottom = &outer;
                    left = &CellGrid[x*cellGridCols + y - 1];

                    topRight = &CellGrid[(x - 1) * cellGridCols + y + 1];
                    bottomRight = &outer;
                    bottomLeft = &outer;
                    topLeft = &CellGrid[(x - 1) * cellGridCols + y - 1];

                    type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                    type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
                }
            }
            else if (y == 0)	//left boundary (without corners)
            {
                /*Cell* top = &CellGrid[x - 1][y];
                Cell* right = &CellGrid[x][y + 1];
                Cell* bottom = &CellGrid[x + 1][y];
                Cell* left = &outer;*/

                top = &CellGrid[(x-1)*cellGridCols + y];
                right = &CellGrid[x*cellGridCols + y + 1];
                bottom = &CellGrid[(x+1)*cellGridCols + y];
                left = &outer;

                topRight = &CellGrid[(x - 1) * cellGridCols + y + 1];
                bottomRight = &CellGrid[(x + 1) * cellGridCols + y + 1];
                bottomLeft = &outer;
                topLeft = &outer;

                type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
            }
            else	//right boundary (without corners)
            {
                /*Cell* top = &CellGrid[x - 1][y];
                Cell* right = &outer;
                Cell* bottom = &CellGrid[x + 1][y];
                Cell* left = &CellGrid[x][y - 1];*/

                top = &CellGrid[(x-1)*cellGridCols + y];
                right = &outer;
                bottom = &CellGrid[(x+1)*cellGridCols + y];
                left = &CellGrid[x*cellGridCols + y - 1];

                topRight = &outer;
                bottomRight = &outer;
                bottomLeft = &CellGrid[(x + 1) * cellGridCols + y - 1];
                topLeft = &CellGrid[(x - 1) * cellGridCols + y - 1];

                type = TypeOfNeighbourhood(*top, *right, *bottom, *left);
                type_slant = TypeOfNeighbourhood(*topRight, *bottomRight, *bottomLeft, *topLeft);
            }
        }

        cell->SetNeighbours(top, right, bottom, left);
        cell->SetTypeOfNeighbourhood(type);
        cell->SetNeighboursOnSlant(topRight, bottomRight, bottomLeft, topLeft);
        cell->SetTypeOfNeighbourhoodOnSlant(type_slant);
    }

    //cout << "Przydzielanie sasiadow zakonczone\n";
}

int Solver::TypeOfNeighbourhood(Cell top, Cell right, Cell bottom, Cell left)	//(*topRight, *bottomRight, *bottomLeft, *topLeft) for slant
{
    bool t = top.GetFluid();
    bool r = right.GetFluid();
    bool b = bottom.GetFluid();
    bool l = left.GetFluid();

    if( t && r && b && l )
        return 0;
    if (!t && r && b && l)
        return 1;
    if (t && !r && b && l)
        return 2;
    if (t && r && !b && l)
        return 3;
    if (t && r && b && !l)
        return 4;
    if (!t && !r && b && l)
        return 5;
    if (!t && r && !b && l)
        return 6;
    if (!t && r && b && !l)
        return 7;
    if (t && !r && !b && l)
        return 8;
    if (t && !r && b && !l)
        return 9;
    if (t && r && !b && !l)
        return 10;
    if (!t && !r && !b && l)
        return 11;
    if (t && !r && !b && !l)
        return 12;
    if (!t && r && !b && !l)
        return 13;
    if (!t && !r && b && !l)
        return 14;

    return 15;
}

void Solver::UpdateGrid()
{
    for (int i = 0; i < FluidCells.size(); i++)
    {
        FluidCells[i]->Update();
        if (FluidCells[i]->GetSource())
        {
            switch(start_direction)
            {
            case 1:
                FluidCells[i]->SetLeftInput(this->v_start);
                break;
            case 2:
                FluidCells[i]->SetRightInput(this->v_start);
                break;
            case 3:
                FluidCells[i]->SetTopInput(this->v_start);
                break;
            case 4:
                FluidCells[i]->SetBottomInput(this->v_start);
                break;
            }
        }
    }
}

void Solver::ShowStep()
{
    for (int i = 0; i < cellGridRows; i++)
    {
        for (int j = 0; j < cellGridCols; j++)
        {
            if( CellGrid[i*cellGridCols+j].GetFluid() )
            //if( CellGrid[i][j].GetFluid() )
                //cout << CellGrid[i][j].GetFluidAmount() << " ";
                //cout << CellGrid[i*cellGridCols+j].GetFluidAmount() << " ";
                //printf("%.0f ", CellGrid[i * cellGridCols + j].GetFluidAmount() );
                printf("%.0f  ", CellGrid[i * cellGridCols + j].GetVelocity());
            else
                cout << "  ";
        }
        cout << endl;
    }
    cout << endl << endl;
}

void Solver::Standarization()
{
    for (int j = 0; j < cellGridCols; j++)
    {
        double fluid_amount = 0.0;
        for (int i = 0; i < cellGridRows; i++)
        {
            if( CellGrid[i*cellGridCols + j].GetFluid() )
                //fluid_amount += CellGrid[i * cellGridCols + j].GetFluidAmount();
                fluid_amount += CellGrid[i * cellGridCols + j].GetVelocity();
        }

        if (abs(fluid_input - fluid_amount) > 1e-3 && abs(fluid_amount) > 1e-5 )
        {
            double factor = fluid_input / fluid_amount;
            for (int i = 0; i < cellGridRows; i++)
            {
                if (CellGrid[i * cellGridCols + j].GetFluid())
                    CellGrid[i * cellGridCols + j].StandarizeCell(factor);
            }
        }
    }

}

void Solver::RemoveExtremalPoints()
{
    double sum = 0.0;
    double mean = 0.0;
    double sigma = 0.0;
    int nExtremal = 1;

    for (int i = 0; i < FluidCells.size(); i++)
        sum += FluidCells[i]->GetVelocity();

    mean = sum / FluidCells.size();
    sum = 0.0;

    for (int i = 0; i < FluidCells.size(); i++)
    {
        double temp = ( FluidCells[i]->GetVelocity() - mean ) * ( FluidCells[i]->GetVelocity() - mean );
        sum += temp;
    }

    sigma = sqrt( sum / (double)FluidCells.size() );

    while (nExtremal != 0)
    {
        nExtremal = 0;
        for (int i = 0; i < FluidCells.size(); i++)
        {
            if (FluidCells[i]->GetVelocity() > mean + 2 * sigma)
            {
                nExtremal++;
                FluidCells[i]->GetMeanFromNeighbours(mean + 2*sigma);
                //FluidCells[i]->StandarizeCell( mean / FluidCells[i]->GetVelocity() );
            }
        }
    }
}
